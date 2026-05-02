"""Windows file-I/O benchmark via ctypes → CreateFileA / WriteFile / ReadFile."""
import ctypes
import ctypes.wintypes
import os
import sys
import tempfile
import time

k32 = ctypes.windll.kernel32

GENERIC_READ    = 0x80000000
GENERIC_WRITE   = 0x40000000
CREATE_ALWAYS   = 2
FILE_ATTRIBUTE_NORMAL = 0x80
FILE_BEGIN      = 0
INVALID_HANDLE  = ctypes.wintypes.HANDLE(-1).value

ITERATIONS = 200
DATA       = b'A' * 1024
write_buf  = ctypes.create_string_buffer(DATA)
read_buf   = ctypes.create_string_buffer(1024)
tmp_dir    = tempfile.gettempdir().encode()
total      = 0.0

for i in range(ITERATIONS):
    path = os.path.join(tempfile.gettempdir(), f'wbench_{i}.tmp').encode()

    t0 = time.perf_counter()

    h = k32.CreateFileA(path,
                        GENERIC_READ | GENERIC_WRITE,
                        0, None, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, None)
    if h == INVALID_HANDLE:
        print(f'CreateFile failed: {k32.GetLastError()}', file=sys.stderr)
        sys.exit(1)

    n = ctypes.wintypes.DWORD()
    k32.WriteFile(h, write_buf, 1024, ctypes.byref(n), None)
    k32.SetFilePointer(h, 0, None, FILE_BEGIN)
    k32.ReadFile(h, read_buf, 1024, ctypes.byref(n), None)
    k32.CloseHandle(h)
    k32.DeleteFileA(path)

    t1 = time.perf_counter()
    total += t1 - t0

print(f'RESULT:{(total / ITERATIONS) * 1000:.4f}')
