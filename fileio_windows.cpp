#include <iostream>
#include <windows.h>
using namespace std;

const int   ITERATIONS  = 50;
const DWORD BUFFER_SIZE = 4096;

int main() {
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);

    char writeBuffer[BUFFER_SIZE];
    char readBuffer[BUFFER_SIZE];
    memset(writeBuffer, 'A', BUFFER_SIZE);

    double totalOpen  = 0.0;
    double totalWrite = 0.0;
    double totalRead  = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {

        QueryPerformanceCounter(&start);
        HANDLE hFile = CreateFileA(
            "test.txt",
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL
        );
        QueryPerformanceCounter(&end);
        if (hFile == INVALID_HANDLE_VALUE) {
            cerr << "CreateFile failed. Error: " << GetLastError() << endl;
            return 1;
        }
        totalOpen += static_cast<double>(end.QuadPart - start.QuadPart) /
                     frequency.QuadPart * 1000.0;

        DWORD bytesWritten = 0;
        QueryPerformanceCounter(&start);
        WriteFile(hFile, writeBuffer, BUFFER_SIZE, &bytesWritten, NULL);
        QueryPerformanceCounter(&end);
        totalWrite += static_cast<double>(end.QuadPart - start.QuadPart) /
                      frequency.QuadPart * 1000.0;

        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

        DWORD bytesRead = 0;
        QueryPerformanceCounter(&start);
        ReadFile(hFile, readBuffer, BUFFER_SIZE, &bytesRead, NULL);
        QueryPerformanceCounter(&end);
        totalRead += static_cast<double>(end.QuadPart - start.QuadPart) /
                     frequency.QuadPart * 1000.0;

        CloseHandle(hFile);
    }

    DeleteFileA("test.txt");

    cout << "=== Windows File I/O Benchmark ("
         << ITERATIONS << " iterations, "
         << BUFFER_SIZE << " bytes) ===" << endl;
    cout << "Average CreateFile() time : " << totalOpen  / ITERATIONS << " ms" << endl;
    cout << "Average WriteFile() time  : " << totalWrite / ITERATIONS << " ms" << endl;
    cout << "Average ReadFile() time   : " << totalRead  / ITERATIONS << " ms" << endl;

    return 0;
}