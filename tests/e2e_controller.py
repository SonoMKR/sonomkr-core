#!/usr/bin/env python3
"""End-to-end test of the controller socket (REQ/REP) of sonomkr-core.

sonomkr-core reads a looped WAV file (1 kHz at 94 dB SPL, channel 1 active, channel 2 inactive).
The test sends invalid and valid commands and checks that:
  - every command gets a reply and the process stays alive,
  - invalid sensitivities are refused and leave the setting unchanged,
  - a valid sensitivity is applied to the published levels.

Usage:
    e2e_controller.py --core build/sonomkr-core --filters conf/filters.conf
"""

import argparse
import math
import os
import shutil
import subprocess
import sys
import tempfile
import time

import zmq

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import wavgen  # noqa: E402
from e2e_wav import CONFIG_TEMPLATE, DEFAULTS, energy_mean, parse_spectrum  # noqa: E402

TONE_BAND, TONE_LEVEL = 31, 94.0  # 1 kHz
SENSITIVITY = 0.05  # V/Pa, initial value
REPLY_TIMEOUT_MS = 5000


class Core:
    def __init__(self, core, filters, workdir):
        self.workdir = workdir
        wav = os.path.join(workdir, "input.wav")
        signal = wavgen.synth(48000, 2.0, [(wavgen.band_frequency(TONE_BAND), TONE_LEVEL)], sensitivity=SENSITIVITY)
        wavgen.write_wav(wav, 48000, [signal], "s16")

        self.ep = {k: "ipc://" + os.path.join(workdir, k) for k in ("ctrl", "audio", "ch1", "ch2")}
        sc = dict(DEFAULTS, rate=48000, sensitivity=SENSITIVITY)
        self.config_path = os.path.join(workdir, "sonomkr.conf")
        with open(self.config_path, "w") as f:
            f.write(CONFIG_TEMPLATE.format(nch=1, ch1_active="true", ch2_active="false", **self.ep, **sc))

        self.log_path = os.path.join(workdir, "core.log")
        self.log = open(self.log_path, "w")
        # real time x4, looped: spectra keep coming while the commands are sent
        self.proc = subprocess.Popen([core, "--config", self.config_path, "--filters", filters,
                                      "--wav", wav, "--wav-speed", "4", "--wav-loop"],
                                     stdout=self.log, stderr=subprocess.STDOUT, cwd=workdir)
        self.ctx = zmq.Context()
        self.sub = self.ctx.socket(zmq.SUB)
        self.sub.setsockopt(zmq.SUBSCRIBE, b"LEQ")
        self.sub.connect(self.ep["ch1"])
        self.req = None
        self._connect_req()
        time.sleep(0.5)  # slow joiner

    def _connect_req(self):
        # a REQ socket is unusable after a missing reply: recreate it
        if self.req is not None:
            self.req.close(linger=0)
        self.req = self.ctx.socket(zmq.REQ)
        self.req.setsockopt(zmq.RCVTIMEO, REPLY_TIMEOUT_MS)
        self.req.connect(self.ep["ctrl"])

    def alive(self):
        return self.proc.poll() is None

    def command(self, *parts):
        """Send a command, return the reply parts as strings, or None (no reply)."""
        self.req.send_multipart([p.encode() for p in parts])
        try:
            return [p.decode() for p in self.req.recv_multipart()]
        except zmq.Again:
            self._connect_req()
            return None

    def level(self, band, count=4):
        """Energy mean of `band` over the next `count` spectra, after dropping the pending ones."""
        while self.sub.poll(0):
            self.sub.recv_multipart()
        self.sub.recv_multipart()  # may straddle the last command
        values = []
        poller = zmq.Poller()
        poller.register(self.sub, zmq.POLLIN)
        while len(values) < count:
            if not poller.poll(5000):
                raise RuntimeError("no spectrum received")
            values.append(parse_spectrum(self.sub.recv_multipart()[1])[1][band])
        return energy_mean(values)

    def close(self):
        self.proc.terminate()
        try:
            self.proc.wait(5)
        except subprocess.TimeoutExpired:
            self.proc.kill()
            self.proc.wait()
        self.log.close()
        self.ctx.destroy(linger=0)


def run(core_path, filters, keep=False):
    workdir = tempfile.mkdtemp(prefix="sonomkr-ctrl-")
    core = Core(core_path, filters, workdir)
    failures = []

    def check(label, ok, detail=""):
        print("%-55s %s %s" % (label, "ok" if ok else "FAIL", detail))
        if not ok:
            failures.append("%s %s" % (label, detail))

    try:
        reply = core.command("START", "ALL")
        check("START ALL", reply is not None and reply[0] == "200", repr(reply))
        reference = core.level(TONE_BAND)
        check("initial level", abs(reference - TONE_LEVEL) <= 0.5, "%.2f dB" % reference)

        # invalid values: refused with 400, process alive, setting unchanged
        for value in ("abc", "", "0.05abc", "0", "-0.05", "nan", "inf", "1e999"):
            label = "SET 1 SENSITIVITY %r" % value
            reply = core.command("SET", "1", "SENSITIVITY", value)
            check(label, reply is not None and reply[0] == "400" and core.alive(), repr(reply))
            if not core.alive():
                break
            reply = core.command("GET", "1", "SENSITIVITY")
            check("  sensitivity unchanged", reply is not None and float(reply[2]) == SENSITIVITY, repr(reply))

        # inactive channel: no processing to update, the setting is saved for later
        if core.alive():
            reply = core.command("SET", "2", "SENSITIVITY", "0.02")
            check("SET 2 SENSITIVITY 0.02 (channel 2 inactive)",
                  reply is not None and reply[0] == "200" and core.alive(), repr(reply))
        if core.alive():
            reply = core.command("GET", "2", "SENSITIVITY")
            check("  GET 2 SENSITIVITY", reply is not None and abs(float(reply[2]) - 0.02) < 1e-9, repr(reply))

        # valid value: twice the sensitivity = levels 6.02 dB lower
        if core.alive():
            reply = core.command("SET", "1", "SENSITIVITY", "0.1")
            check("SET 1 SENSITIVITY 0.1", reply is not None and reply[0] == "200", repr(reply))
            level = core.level(TONE_BAND)
            expected = TONE_LEVEL - 20 * math.log10(0.1 / SENSITIVITY)
            check("  level after SET", abs(level - expected) <= 0.5, "%.2f dB, expected %.2f" % (level, expected))
            with open(core.config_path) as f:
                saved = f.read()
            check("  saved in the configuration file", "sensitivity = 0.1" in saved)

        check("process still running", core.alive(),
              "" if core.alive() else "exit code %s" % core.proc.returncode)
    except Exception as exc:  # noqa: BLE001
        failures.append("exception: %s" % exc)
    finally:
        core.close()

    if failures:
        print("\nFAILED controller:")
        for f in failures:
            print("  - " + f)
        print("\n--- sonomkr-core log ---")
        with open(core.log_path) as f:
            print(f.read())
    else:
        print("PASSED controller")
    if keep:
        print("work directory kept: " + workdir)
    else:
        shutil.rmtree(workdir, ignore_errors=True)
    return not failures


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--core", required=True, help="path to the sonomkr-core executable")
    p.add_argument("--filters", required=True, help="path to filters.conf")
    p.add_argument("--keep", action="store_true", help="keep the work directory (WAV, config, log)")
    args = p.parse_args()
    return 0 if run(os.path.abspath(args.core), os.path.abspath(args.filters), args.keep) else 1


if __name__ == "__main__":
    sys.exit(main())
