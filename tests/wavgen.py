#!/usr/bin/env python3
"""Synthetic WAV generator for the sonomkr-core tests (standard library only).

Levels are expressed in dB SPL, converted to digital full scale with the same
convention as sonomkr-core: 1.0 full scale = 1 V, sensitivity in V/Pa, so
    Leq = 20 log10(rms) - 20 log10(sensitivity * P0)

Command line example (1 kHz at 94 dB, 10 s, 48 kHz, 24 bit):
    python3 wavgen.py out.wav --rate 48000 --format s24 --duration 10 --tone 1000:94
"""

import argparse
import math
import random
import struct
import sys
from array import array

P0 = 2e-5
DEFAULT_SENSITIVITY = 0.05  # V/Pa, default of conf/sonomkr.conf

FORMATS = {
    # name: (wav format tag, bytes per sample)
    "s16": (1, 2),
    "s24": (1, 3),
    "s32": (1, 4),
    "float": (3, 4),
}


def band_frequency(band):
    """Exact (base 10) centre frequency of a sonomkr third-octave band index (31 = 1 kHz)."""
    return 1000.0 * 10.0 ** ((band - 31) / 10.0)


def level_to_amplitude(level_db, sensitivity=DEFAULT_SENSITIVITY):
    """Peak amplitude (full scale) of a sine giving `level_db` dB SPL."""
    return math.sqrt(2.0) * sensitivity * P0 * 10.0 ** (level_db / 20.0)


def synth(rate, duration, tones, noise_db=None, sensitivity=DEFAULT_SENSITIVITY, seed=0):
    """One channel: sum of sines [(freq_hz, level_db), ...] plus optional white noise."""
    n = int(round(rate * duration))
    out = [0.0] * n
    for freq, level in tones:
        a = level_to_amplitude(level, sensitivity)
        w = 2.0 * math.pi * freq / rate
        for i in range(n):
            out[i] += a * math.sin(w * i)
    if noise_db is not None:
        sigma = sensitivity * P0 * 10.0 ** (noise_db / 20.0)  # rms of the white noise
        rnd = random.Random(seed)
        for i in range(n):
            out[i] += rnd.gauss(0.0, sigma)
    peak = max((abs(x) for x in out), default=0.0)
    if peak >= 1.0:
        raise ValueError("signal clips (peak %.3f FS): lower the levels" % peak)
    return out


def write_wav(path, rate, channels, fmt="s16"):
    """Write interleaved channels (list of equal-length float lists) to `path`."""
    tag, width = FORMATS[fmt]
    nch = len(channels)
    n = len(channels[0])
    if any(len(c) != n for c in channels):
        raise ValueError("channels must have the same length")

    interleaved = [channels[c][i] for i in range(n) for c in range(nch)]
    if fmt == "float":
        data = array("f", interleaved)
    elif fmt == "s16":
        data = array("h", (int(round(x * 32767.0)) for x in interleaved))
    elif fmt == "s32":
        data = array("i", (int(round(x * 2147483647.0)) for x in interleaved))
    else:  # s24, packed little endian
        raw = bytearray()
        for x in interleaved:
            v = int(round(x * 8388607.0)) & 0xFFFFFF
            raw += bytes((v & 0xFF, (v >> 8) & 0xFF, (v >> 16) & 0xFF))
        data = raw
    if isinstance(data, array):
        if sys.byteorder != "little":
            data.byteswap()
        data = data.tobytes()

    block_align = nch * width
    fmt_chunk = struct.pack("<HHIIHH", tag, nch, rate, rate * block_align, block_align, width * 8)
    with open(path, "wb") as f:
        f.write(b"RIFF")
        f.write(struct.pack("<I", 4 + 8 + len(fmt_chunk) + 8 + len(data)))
        f.write(b"WAVE")
        f.write(b"fmt " + struct.pack("<I", len(fmt_chunk)) + fmt_chunk)
        f.write(b"data" + struct.pack("<I", len(data)))
        f.write(data)


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("output")
    p.add_argument("--rate", type=int, default=48000)
    p.add_argument("--format", choices=sorted(FORMATS), default="s16")
    p.add_argument("--duration", type=float, default=10.0, help="seconds")
    p.add_argument("--channels", type=int, default=1)
    p.add_argument("--tone", action="append", default=[], metavar="FREQ:DB",
                   help="sine at FREQ Hz and DB dB SPL (repeatable)")
    p.add_argument("--noise", type=float, default=None, metavar="DB", help="white noise level, dB SPL")
    p.add_argument("--sensitivity", type=float, default=DEFAULT_SENSITIVITY, help="V/Pa")
    args = p.parse_args()

    tones = [tuple(float(v) for v in t.split(":")) for t in args.tone]
    signal = synth(args.rate, args.duration, tones, args.noise, args.sensitivity)
    write_wav(args.output, args.rate, [signal] * args.channels, args.format)
    print("written %s (%d Hz, %s, %d ch, %.1f s)" % (args.output, args.rate, args.format,
                                                     args.channels, args.duration))


if __name__ == "__main__":
    main()
