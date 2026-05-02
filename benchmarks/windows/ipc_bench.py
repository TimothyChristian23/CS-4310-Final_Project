"""Windows IPC benchmark via ctypes → CreatePipe / WriteFile / ReadFile."""
import ctypes
import ctypes.wintypes
import sys
import time

k32 = ctypes.windll.kernel32

class SECURITY_ATTRIBUTES(ctypes.Structure):
    _fields_ = [
        ('nLength',              ctypes.wintypes.DWORD),
        ('lpSecurityDescriptor', ctypes.c_void_p),
        ('bInheritHandle',       ctypes.wintypes.BOOL),
    ]

ITERATIONS = 1000
MSG        = b'X' * 1024
send_buf   = ctypes.create_string_buffer(MSG)
recv_buf   = ctypes.create_string_buffer(1024)
total      = 0.0

for _ in range(ITERATIONS):
    sa = SECURITY_ATTRIBUTES()
    sa.nLength = ctypes.sizeof(sa)
    sa.bInheritHandle = True

    hRead  = ctypes.wintypes.HANDLE()
    hWrite = ctypes.wintypes.HANDLE()

    if not k32.CreatePipe(ctypes.byref(hRead), ctypes.byref(hWrite),
                          ctypes.byref(sa), 1024 * 2):
        print(f'CreatePipe failed: {k32.GetLastError()}', file=sys.stderr)
        sys.exit(1)

    t0 = time.perf_counter()
    n = ctypes.wintypes.DWORD()
    k32.WriteFile(hWrite, send_buf, 1024, ctypes.byref(n), None)
    k32.ReadFile(hRead, recv_buf,  1024, ctypes.byref(n), None)
    t1 = time.perf_counter()

    total += t1 - t0
    k32.CloseHandle(hRead)
    k32.CloseHandle(hWrite)

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
