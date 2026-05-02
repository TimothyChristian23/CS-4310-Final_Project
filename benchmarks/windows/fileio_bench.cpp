#include <windows.h>
#include <iostream>
using namespace std;

const int ITERATIONS = 200;
const int BUF_SIZE = 1024;

int main() {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    double total = 0.0;

    char writeBuf[BUF_SIZE], readBuf[BUF_SIZE];
    memset(writeBuf, 'A', BUF_SIZE);

    char tmpPath[MAX_PATH], tmpFile[MAX_PATH];
    GetTempPathA(MAX_PATH, tmpPath);

    for (int i = 0; i < ITERATIONS; i++) {
        GetTempFileNameA(tmpPath, "bch", 0, tmpFile);

        QueryPerformanceCounter(&start);

        HANDLE hFile = CreateFileA(tmpFile,
            GENERIC_READ | GENERIC_WRITE, 0,
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            cerr << "CreateFile failed: " << GetLastError() << "\n";
            return 1;
        }

        DWORD n;
        WriteFile(hFile, writeBuf, BUF_SIZE, &n, NULL);
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        ReadFile(hFile, readBuf, BUF_SIZE, &n, NULL);
        CloseHandle(hFile);
        DeleteFileA(tmpFile);

        QueryPerformanceCounter(&end);
        total += (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    }

    cout << "RESULT:" << (total / ITERATIONS) * 1000 << endl;
    return 0;
}
