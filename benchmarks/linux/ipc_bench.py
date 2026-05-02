"""Linux IPC benchmark: pipe() / write() / read()."""
import os
import time

ITERATIONS = 1000
MSG        = b'X' * 1024
total      = 0.0

for _ in range(ITERATIONS):
    r, w = os.pipe()

    t0 = time.perf_counter()
    os.write(w, MSG)
    os.read(r, 1024)
    t1 = time.perf_counter()

    total += t1 - t0
    os.close(r)
    os.close(w)

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
