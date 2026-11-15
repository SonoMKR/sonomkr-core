#!/usr/bin/env python3
"""End-to-end test: synthetic WAV -> sonomkr-core -> third-octave spectra over ZeroMQ.

For one scenario:
  1. generate a WAV file with known tones (wavgen.py),
  2. write a temporary configuration (autostart off, ipc:// endpoints),
  3. start `sonomkr-core --wav <file> --wav-speed 0`,
  4. subscribe to the LEQ topic of each channel and send START ALL on the controller,
  5. average the received spectra (energy average, first ones skipped)
     and compare each tone band with its expected level.

Usage:
    e2e_wav.py --core build/sonomkr-core --filters conf/filters.conf --scenario sine_1k_s16
    e2e_wav.py --list
"""

import argparse
import math
import os
import subprocess
import sys
import tempfile
import time
from array import array

import zmq

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import wavgen  # noqa: E402

B = wavgen.band_frequency

# Each channel lists its tones as (band index, level dB SPL).
# tol: allowed deviation on tone bands (dB).
# floor: bands at least 3 bands away from every tone must be this many dB below the loudest tone.
SCENARIOS = {
    "sine_1k_s16": dict(rate=48000, fmt="s16", channels=[[(31, 94.0)]]),
    "sine_1k_s24": dict(rate=48000, fmt="s24", channels=[[(31, 94.0)]]),
    "sine_1k_s32": dict(rate=48000, fmt="s32", channels=[[(31, 94.0)]]),
    "sine_1k_float": dict(rate=48000, fmt="float", channels=[[(31, 94.0)]]),
    "stereo_two_channels": dict(rate=48000, fmt="s16", channels=[[(31, 94.0)], [(25, 80.0)]]),
    "multitone_48k": dict(rate=48000, fmt="s24",
                          channels=[[(16, 85.0), (21, 80.0), (26, 75.0), (31, 85.0), (36, 80.0), (41, 75.0)]]),
    # below 2 Hz the bands are only ~0.2 Hz wide: with 1 s integration the
    # estimate fluctuates a lot, hence the relaxed floor
    "low_bands_48k": dict(rate=48000, fmt="s24", fmin=0, duration=30.0, integration=1.0, skip=5, floor=25.0,
                          channels=[[(4, 90.0), (12, 90.0), (18, 85.0)]]),
    # at 44.1 kHz there are two decimation stages only (44100 % 1000 != 0): bands from 8 Hz
    "multitone_44k1": dict(rate=44100, fmt="s16",
                           channels=[[(13, 85.0), (21, 85.0), (31, 85.0), (40, 80.0)]]),
    # integration shorter than a 10000 sample input block (~0.21 s at 48 kHz)
    "short_integration": dict(rate=48000, fmt="s16", integration=0.125, skip=8, channels=[[(31, 94.0)]]),
    # longer than the 15 s the WAV source may get ahead of the filters
    "long_file_60s": dict(rate=48000, fmt="s16", duration=60.0, channels=[[(31, 94.0)], [(21, 80.0)]]),
    # 20 kHz band at 44.1 kHz: its upper edge (22.4 kHz) is above Nyquist (22.05 kHz)
    "band_20k_44k1": dict(rate=44100, fmt="s24", duration=20.0,
                          channels=[[(44, 80.0), (31, 70.0)], [(31, 80.0)]]),
    # paced playback (4x real time) instead of "as fast as possible"
    "paced_x4": dict(rate=48000, fmt="s16", speed=4.0, channels=[[(28, 70.0), (34, 75.0)]]),
    # only channel 2 active: it must process the second input of the file
    "channel2_only": dict(rate=48000, fmt="s16", active=(False, True), channels=[[(31, 94.0)], [(25, 80.0)]]),
    # mono file, only channel 2 active: the file feeds channel 2
    "channel2_only_mono": dict(rate=48000, fmt="s16", active=(False, True), channels=[[(28, 85.0)]]),
}

DEFAULTS = dict(fmin=12, fmax=44, duration=8.0, integration=0.5, sensitivity=0.05, skip=2, tol=0.5, floor=30.0,
                speed=0.0, active=None)

# the last, incomplete integration period of the file is not emitted
TAIL_LOSS = 1  # spectra
IDLE_TIMEOUT = 3.0  # s without new spectrum = end of the stream

CONFIG_TEMPLATE = """version = "1.0";
autostart = false;
controllerBind = "{ctrl}";
audio :
{{
  soundCard = "none";
  availableChannels = {nch};
  sampleRate = {rate};
  format = "S16_LE";
  periods = 4;
  periodSize = 4096;
  publishBind = "{audio}";
}};
channel1 :
{{
  active = {ch1_active};
  strategy = "G10";
  fmin = {fmin};
  fmax = {fmax};
  integrationPeriod = {integration:.3f};
  sensitivity = {sensitivity:.6f};
  publishBind = "{ch1}";
}};
channel2 :
{{
  active = {ch2_active};
  strategy = "G10";
  fmin = {fmin};
  fmax = {fmax};
  integrationPeriod = {integration:.3f};
  sensitivity = {sensitivity:.6f};
  publishBind = "{ch2}";
}};
"""


def parse_spectrum(payload):
    """'2026-01-01 12:00:00.000;12:45.10;13:46.00;' -> (timestamp, {band: level})"""
    fields = payload.decode().strip(";").split(";")
    levels = {}
    for item in fields[1:]:
        band, value = item.split(":")
        levels[int(band)] = float(value)
    return fields[0], levels


def audio_rms_db(payloads):
    """Level (dB re 1 FS rms) of the float32 audio blocks published on the audio socket."""
    samples = array("f")
    for p in payloads:
        block = array("f")
        block.frombytes(p)
        samples.extend(block)
    if sys.byteorder != "little":
        samples.byteswap()
    ms = sum(x * x for x in samples) / max(len(samples), 1)
    return 10.0 * math.log10(ms) if ms > 0 else -999.0


def energy_mean(values):
    return 10.0 * math.log10(sum(10.0 ** (v / 10.0) for v in values) / len(values))


def run_scenario(name, core, filters, keep=False, verbose=False):
    sc = dict(DEFAULTS)
    sc.update(SCENARIOS[name])
    nch = len(sc["channels"])  # inputs in the WAV file
    active = list(sc["active"] or [True, nch > 1])
    chans = [c for c in range(2) if active[c]]  # active channels (0 = channel 1)
    workdir = tempfile.mkdtemp(prefix="sonomkr-e2e-")

    # 1. WAV
    wav_path = os.path.join(workdir, "input.wav")
    signals = [wavgen.synth(sc["rate"], sc["duration"], [(B(b), lvl) for b, lvl in tones],
                            sensitivity=sc["sensitivity"])
               for tones in sc["channels"]]
    wavgen.write_wav(wav_path, sc["rate"], signals, sc["fmt"])

    # 2. configuration (written back by sonomkr-core, hence the temporary copy)
    ep = {k: "ipc://" + os.path.join(workdir, k) for k in ("ctrl", "audio", "ch1", "ch2")}
    config_path = os.path.join(workdir, "sonomkr.conf")
    with open(config_path, "w") as f:
        f.write(CONFIG_TEMPLATE.format(nch=nch, ch1_active=str(active[0]).lower(),
                                       ch2_active=str(active[1]).lower(), **ep, **sc))

    # 3. core
    log_path = os.path.join(workdir, "core.log")
    log = open(log_path, "w")
    proc = subprocess.Popen([core, "--config", config_path, "--filters", filters,
                             "--wav", wav_path, "--wav-speed", str(sc["speed"])],
                            stdout=log, stderr=subprocess.STDOUT, cwd=workdir)

    ctx = zmq.Context()
    spectra = [[] for _ in chans]
    audio = {}  # topic -> list of payloads
    full_count = int(sc["duration"] / sc["integration"])
    min_count = max(full_count - TAIL_LOSS, sc["skip"] + 3)
    error = None
    try:
        # 4. subscribe, then start the channels through the controller
        subs = []
        for c in chans:
            s = ctx.socket(zmq.SUB)
            s.setsockopt(zmq.SUBSCRIBE, b"LEQ")
            s.connect(ep["ch%d" % (c + 1)])
            subs.append(s)
        audio_sub = ctx.socket(zmq.SUB)
        audio_sub.setsockopt(zmq.SUBSCRIBE, b"")
        audio_sub.connect(ep["audio"])
        req = ctx.socket(zmq.REQ)
        req.setsockopt(zmq.RCVTIMEO, 10000)
        req.connect(ep["ctrl"])
        time.sleep(0.5)  # let the SUB sockets join (slow joiner)
        req.send_multipart([b"START", b"ALL"])
        reply = req.recv_multipart()
        if reply[0] != b"200":
            raise RuntimeError("START refused: %r" % reply)

        poller = zmq.Poller()
        for s in subs + [audio_sub]:
            poller.register(s, zmq.POLLIN)
        # collect until every spectrum of the file is received, or the stream stops
        started = time.time()
        deadline = started + 60 + sc["duration"] / (sc["speed"] if sc["speed"] > 0 else 10.0)
        last_rx = started
        while time.time() < deadline and min(len(s) for s in spectra) < full_count:
            if proc.poll() is not None:
                raise RuntimeError("sonomkr-core exited with code %d" % proc.returncode)
            if min(len(s) for s in spectra) > 0 and time.time() - last_rx > IDLE_TIMEOUT:
                break
            for sock, _ in poller.poll(200):
                topic, payload = sock.recv_multipart()
                if sock is audio_sub:
                    audio.setdefault(topic.decode(), []).append(payload)
                    continue
                spectra[subs.index(sock)].append(parse_spectrum(payload)[1])
                last_rx = time.time()
        counts = [len(s) for s in spectra]
        print("%s: %s spectra received per channel (file: %d, minimum: %d) in %.1f s"
              % (name, counts, full_count, min_count, time.time() - started))
        if min(counts) < min_count:
            error = "received %s spectra, expected at least %d per channel" % (counts, min_count)
    except Exception as exc:  # noqa: BLE001
        error = str(exc)
    finally:
        proc.terminate()
        try:
            proc.wait(5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()
        log.close()
        ctx.destroy(linger=0)

    # 5. checks
    failures = []
    if error:
        failures.append(error)
    # audio stream: one topic per active channel, carrying the matching input of the file
    expected_topics = ["CH%d" % (c + 1) for c in chans]
    if sorted(audio) != expected_topics:
        failures.append("audio topics %s, expected %s" % (sorted(audio), expected_topics))
    for c in chans:
        tones = sc["channels"][c if c < nch else 0]
        exp = 10.0 * math.log10(sum(wavgen.level_to_amplitude(lvl, sc["sensitivity"]) ** 2 / 2 for _, lvl in tones))
        got = audio_rms_db(audio.get("CH%d" % (c + 1), []))
        print("audio CH%d: %.2f dB FS rms, expected %.2f" % (c + 1, got, exp))
        if abs(got - exp) > sc["tol"]:
            failures.append("audio CH%d: %.2f dB FS rms, expected %.2f +/- %.2f" % (c + 1, got, exp, sc["tol"]))

    for i, c in enumerate(chans):
        tones = sc["channels"][c if c < nch else 0]
        received = spectra[i][sc["skip"]:]
        if not received:
            failures.append("channel %d: no usable spectrum" % (c + 1))
            continue
        bands = sorted(received[0])
        mean = {b: energy_mean([s[b] for s in received]) for b in bands}
        spread = {b: max(s[b] for s in received) - min(s[b] for s in received) for b in bands}
        tone_levels = dict(tones)
        loudest = max(tone_levels.values())

        print("channel %d - %d spectra averaged (%d skipped)" % (c + 1, len(received), sc["skip"]))
        print("  band  freq(Hz)   Leq(dB)  spread  expected")
        for b in bands:
            exp = tone_levels.get(b)
            far = all(abs(b - t) >= 3 for t in tone_levels)
            status = ""
            if exp is not None:
                if abs(mean[b] - exp) > sc["tol"]:
                    status = "FAIL"
                    failures.append("channel %d band %d: %.2f dB, expected %.2f +/- %.2f"
                                    % (c + 1, b, mean[b], exp, sc["tol"]))
            elif far and mean[b] > loudest - sc["floor"]:
                status = "FAIL"
                failures.append("channel %d band %d: %.2f dB, should be below %.2f dB"
                                % (c + 1, b, mean[b], loudest - sc["floor"]))
            if verbose or exp is not None or status:
                print("  %4d  %8.1f  %8.2f  %6.2f  %8s %s" % (
                    b, B(b), mean[b], spread[b], "%.2f" % exp if exp is not None else "", status))

    if failures:
        print("\nFAILED %s:" % name)
        for f in failures:
            print("  - " + f)
        print("\n--- sonomkr-core log ---")
        with open(log_path) as f:
            print(f.read())
    else:
        print("PASSED %s" % name)

    if keep:
        print("work directory kept: " + workdir)
    else:
        for entry in os.listdir(workdir):
            try:
                os.remove(os.path.join(workdir, entry))
            except OSError:
                pass
        os.rmdir(workdir)
    return not failures


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--core", help="path to the sonomkr-core executable")
    p.add_argument("--filters", help="path to filters.conf")
    p.add_argument("--scenario", action="append", help="scenario name (repeatable, default: all)")
    p.add_argument("--list", action="store_true", help="list the scenarios")
    p.add_argument("--keep", action="store_true", help="keep the work directory (WAV, config, log)")
    p.add_argument("-v", "--verbose", action="store_true", help="print every band")
    args = p.parse_args()

    if args.list:
        for name in SCENARIOS:
            print(name)
        return 0
    if not args.core or not args.filters:
        p.error("--core and --filters are required")

    ok = True
    for name in args.scenario or list(SCENARIOS):
        ok &= run_scenario(name, os.path.abspath(args.core), os.path.abspath(args.filters),
                           args.keep, args.verbose)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
