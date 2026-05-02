"""Linux file-I/O benchmark: open() / write() / lseek() / read() / unlink()."""
import os
import sys
import tempfile
import time

ITERATIONS = 200
DATA       = b'A' * 1024
total      = 0.0

for _ in range(ITERATIONS):
    fd, path = tempfile.mkstemp()
    os.close(fd)
    os.unlink(path)

    t0 = time.perf_counter()
    fd = os.open(path, os.O_RDWR | os.O_CREAT | os.O_TRUNC, 0o644)
    os.write(fd, DATA)
    os.lseek(fd, 0, os.SEEK_SET)
    os.read(fd, 1024)
    os.close(fd)
    os.unlink(path)
    t1 = time.perf_counter()
    total += t1 - t0

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
