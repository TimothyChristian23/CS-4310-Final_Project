"""Windows process-creation benchmark via ctypes → CreateProcessA."""
import ctypes
import ctypes.wintypes
import sys
import time

k32 = ctypes.windll.kernel32

class STARTUPINFOA(ctypes.Structure):
    _fields_ = [
        ('cb',             ctypes.wintypes.DWORD),
        ('lpReserved',     ctypes.wintypes.LPSTR),
        ('lpDesktop',      ctypes.wintypes.LPSTR),
        ('lpTitle',        ctypes.wintypes.LPSTR),
        ('dwX',            ctypes.wintypes.DWORD),
        ('dwY',            ctypes.wintypes.DWORD),
        ('dwXSize',        ctypes.wintypes.DWORD),
        ('dwYSize',        ctypes.wintypes.DWORD),
        ('dwXCountChars',  ctypes.wintypes.DWORD),
        ('dwYCountChars',  ctypes.wintypes.DWORD),
        ('dwFillAttribute',ctypes.wintypes.DWORD),
        ('dwFlags',        ctypes.wintypes.DWORD),
        ('wShowWindow',    ctypes.wintypes.WORD),
        ('cbReserved2',    ctypes.wintypes.WORD),
        ('lpReserved2',    ctypes.POINTER(ctypes.c_byte)),
        ('hStdInput',      ctypes.wintypes.HANDLE),
        ('hStdOutput',     ctypes.wintypes.HANDLE),
        ('hStdError',      ctypes.wintypes.HANDLE),
    ]

class PROCESS_INFORMATION(ctypes.Structure):
    _fields_ = [
        ('hProcess',   ctypes.wintypes.HANDLE),
        ('hThread',    ctypes.wintypes.HANDLE),
        ('dwProcessId',ctypes.wintypes.DWORD),
        ('dwThreadId', ctypes.wintypes.DWORD),
    ]

INFINITE = 0xFFFFFFFF
ITERATIONS = 20
total = 0.0

for _ in range(ITERATIONS):
    si  = STARTUPINFOA()
    si.cb = ctypes.sizeof(si)
    pi  = PROCESS_INFORMATION()
    cmd = ctypes.create_string_buffer(b'C:\\Windows\\System32\\cmd.exe /c exit')

    t0 = time.perf_counter()
    ok = k32.CreateProcessA(None, cmd, None, None, False, 0,
                            None, None, ctypes.byref(si), ctypes.byref(pi))
    if not ok:
        print(f'CreateProcess failed: {k32.GetLastError()}', file=sys.stderr)
        sys.exit(1)
    k32.WaitForSingleObject(pi.hProcess, INFINITE)
    t1 = time.perf_counter()

    total += t1 - t0
    k32.CloseHandle(pi.hProcess)
    k32.CloseHandle(pi.hThread)

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
