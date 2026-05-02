"""Linux process-creation benchmark: fork() + execl() + waitpid()."""
import os
import sys
import time

ITERATIONS = 20
total      = 0.0

for _ in range(ITERATIONS):
    t0  = time.perf_counter()
    pid = os.fork()
    if pid == 0:
        os.execl('/bin/true', 'true')
        os._exit(1)
    if pid < 0:
        print('fork failed', file=sys.stderr)
        sys.exit(1)
    os.waitpid(pid, 0)
    t1 = time.perf_counter()
    total += t1 - t0

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
