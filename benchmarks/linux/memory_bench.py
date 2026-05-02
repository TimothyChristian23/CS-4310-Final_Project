"""Linux memory benchmark: mmap(MAP_ANONYMOUS) / memset / munmap()."""
import mmap
import time

ITERATIONS = 1000
SIZE       = 1024 * 1024  # 1 MB
total      = 0.0

for _ in range(ITERATIONS):
    t0 = time.perf_counter()
    mm = mmap.mmap(-1, SIZE)        # MAP_ANONYMOUS | MAP_PRIVATE
    mm.write(b'\x00' * SIZE)        # touch every page
    mm.close()                      # munmap
    t1 = time.perf_counter()
    total += t1 - t0

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
