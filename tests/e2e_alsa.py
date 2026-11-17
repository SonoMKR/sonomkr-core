#!/usr/bin/env python3
"""End-to-end test of the ALSA capture path (AudioCapture), on the ALSA "null" device.

The "null" PCM of alsa-lib generates zero samples in capture: no sound card is needed,
but the real capture code runs (open, read loop, stop, release). The test:
  1. starts sonomkr-core on the "null" device and checks that spectra are published,
  2. reloads the configuration (LOAD), which stops and deletes the capture, then recreates it,
  3. stops the channels and checks the process is still alive;
  4. starts sonomkr-core with an unsupported audio format: it must report it and keep
     answering the controller.

Usage:
    e2e_alsa.py --core build/sonomkr-core --filters conf/filters.conf
"""

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
import time

import zmq

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from e2e_wav import CONFIG_TEMPLATE, DEFAULTS  # noqa: E402

REPLY_TIMEOUT_MS = 5000


def write_config(path, ep, fmt="S16_LE"):
    sc = dict(DEFAULTS, rate=48000)
    text = CONFIG_TEMPLATE.format(nch=2, ch1_active="true", ch2_active="true", **ep, **sc)
    text = text.replace('soundCard = "none"', 'soundCard = "null"').replace('format = "S16_LE"', 'format = "%s"' % fmt)
    with open(path, "w") as f:
        f.write(text)


def start_core(core, filters, config, log_path, workdir):
    log = open(log_path, "w")
    proc = subprocess.Popen([core, "--config", config, "--filters", filters],
                            stdout=log, stderr=subprocess.STDOUT, cwd=workdir)
    return proc, log


def stop_core(proc, log):
    proc.terminate()
    try:
        proc.wait(5)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()
    log.close()


def run(core_path, filters, keep=False):
    workdir = tempfile.mkdtemp(prefix="sonomkr-alsa-")
    ep = {k: "ipc://" + os.path.join(workdir, k) for k in ("ctrl", "audio", "ch1", "ch2")}
    failures = []

    def check(label, ok, detail=""):
        print("%-50s %s %s" % (label, "ok" if ok else "FAIL", detail))
        if not ok:
            failures.append("%s %s" % (label, detail))

    ctx = zmq.Context()

    def request(*parts):
        req = ctx.socket(zmq.REQ)
        req.setsockopt(zmq.RCVTIMEO, REPLY_TIMEOUT_MS)
        req.setsockopt(zmq.LINGER, 0)
        req.connect(ep["ctrl"])
        req.send_multipart([p.encode() for p in parts])
        try:
            return [p.decode() for p in req.recv_multipart()]
        except zmq.Again:
            return None
        finally:
            req.close()

    def spectra(count, timeout=5.0):
        sub = ctx.socket(zmq.SUB)
        sub.setsockopt(zmq.SUBSCRIBE, b"LEQ")
        sub.setsockopt(zmq.LINGER, 0)
        sub.connect(ep["ch2"])
        received, end = 0, time.time() + timeout
        while received < count and time.time() < end:
            if sub.poll(200):
                sub.recv_multipart()
                received += 1
        sub.close()
        return received

    logs = []
    try:
        # 1-3: capture life cycle
        config = os.path.join(workdir, "sonomkr.conf")
        write_config(config, ep)
        log_path = os.path.join(workdir, "core.log")
        logs.append(log_path)
        proc, log = start_core(core_path, filters, config, log_path, workdir)
        try:
            time.sleep(0.5)
            reply = request("START", "ALL")
            check("START ALL", reply is not None and reply[0] == "200", repr(reply))
            check("spectra published (channel 2)", spectra(2) == 2)
            reply = request("LOAD", config)
            check("LOAD (capture stopped, deleted, recreated)", reply is not None and reply[0] == "200", repr(reply))
            check("spectra published after LOAD", spectra(2) == 2)
            reply = request("STOP", "ALL")
            check("STOP ALL", reply is not None and reply[0] == "200", repr(reply))
            check("process still running", proc.poll() is None, "exit code %s" % proc.returncode)
        finally:
            stop_core(proc, log)

        # 4: unsupported format
        bad_config = os.path.join(workdir, "bad_format.conf")
        write_config(bad_config, ep, fmt="S8")
        bad_log = os.path.join(workdir, "bad_format.log")
        logs.append(bad_log)
        proc, log = start_core(core_path, filters, bad_config, bad_log, workdir)
        try:
            time.sleep(0.5)
            reply = request("GET", "1", "SENSITIVITY")
            check("unsupported format: controller answers", reply is not None and reply[0] == "200", repr(reply))
            check("unsupported format: process still running", proc.poll() is None,
                  "exit code %s" % proc.returncode)
        finally:
            stop_core(proc, log)
        with open(bad_log) as f:
            text = f.read()
        check("unsupported format: reported", "Unsupported audio format" in text)
    except Exception as exc:  # noqa: BLE001
        failures.append("exception: %s" % exc)
    finally:
        ctx.destroy(linger=0)

    if failures:
        print("\nFAILED alsa:")
        for f in failures:
            print("  - " + f)
        for path in logs:
            print("\n--- %s ---" % os.path.basename(path))
            with open(path) as f:
                print(f.read())
    else:
        print("PASSED alsa")
    if keep:
        print("work directory kept: " + workdir)
    else:
        shutil.rmtree(workdir, ignore_errors=True)
    return not failures


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--core", required=True, help="path to the sonomkr-core executable")
    p.add_argument("--filters", required=True, help="path to filters.conf")
    p.add_argument("--keep", action="store_true", help="keep the work directory (configs, logs)")
    args = p.parse_args()
    return 0 if run(os.path.abspath(args.core), os.path.abspath(args.filters), args.keep) else 1


if __name__ == "__main__":
    sys.exit(main())
