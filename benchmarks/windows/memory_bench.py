"""Windows memory benchmark via ctypes → VirtualAlloc / VirtualFree."""
import ctypes
import sys
import time

k32 = ctypes.windll.kernel32
k32.VirtualAlloc.restype = ctypes.c_void_p

MEM_COMMIT   = 0x00001000
MEM_RESERVE  = 0x00002000
MEM_RELEASE  = 0x00008000
PAGE_READWRITE = 0x04

ITERATIONS = 1000
SIZE       = 1024 * 1024  # 1 MB
total      = 0.0

for _ in range(ITERATIONS):
    t0 = time.perf_counter()

    addr = k32.VirtualAlloc(None, SIZE,
                            MEM_COMMIT | MEM_RESERVE,
                            PAGE_READWRITE)
    if not addr:
        print(f'VirtualAlloc failed: {k32.GetLastError()}', file=sys.stderr)
        sys.exit(1)

    ctypes.memset(addr, 0, SIZE)
    k32.VirtualFree(ctypes.c_void_p(addr), 0, MEM_RELEASE)

    t1 = time.perf_counter()
    total += t1 - t0

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
