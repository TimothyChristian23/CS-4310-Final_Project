#include <iostream>
#include <windows.h>
using namespace std;

const int   ITERATIONS  = 50;
const DWORD BUFFER_SIZE = 4096;
const char* PIPE_NAME   = "\\\\.\\pipe\\ipc_bench";

DWORD WINAPI child_thread(LPVOID lpParam) {
    HANDLE hPipe = CreateFileA(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) return 1;

    char buf[BUFFER_SIZE];
    DWORD bytesRead = 0;

    for (int i = 0; i < ITERATIONS; i++) {
        /* Read message from parent */
        ReadFile(hPipe, buf, BUFFER_SIZE, &bytesRead, NULL);
        /* Write it back to complete the round trip */
        DWORD bytesWritten = 0;
        WriteFile(hPipe, buf, bytesRead, &bytesWritten, NULL);
    }

    CloseHandle(hPipe);
    return 0;
}

int main() {
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);

    /* Create the named pipe server */
    HANDLE hPipe = CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) {
        cerr << "CreateNamedPipe failed. Error: " << GetLastError() << endl;
        return 1;
    }

    /* Spin up child thread to act as the other process */
    HANDLE hThread = CreateThread(NULL, 0, child_thread, NULL, 0, NULL);
    ConnectNamedPipe(hPipe, NULL);

    char sendBuf[BUFFER_SIZE];
    char recvBuf[BUFFER_SIZE];
    memset(sendBuf, 'A', BUFFER_SIZE);

    double totalTime = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {
        DWORD bytesWritten = 0;
        DWORD bytesRead    = 0;

        QueryPerformanceCounter(&start);

        /* Send to child */
        WriteFile(hPipe, sendBuf, BUFFER_SIZE, &bytesWritten, NULL);
        /* Wait for child to echo back */
        ReadFile(hPipe, recvBuf, BUFFER_SIZE, &bytesRead, NULL);

        QueryPerformanceCounter(&end);

        totalTime += static_cast<double>(end.QuadPart - start.QuadPart) /
                     frequency.QuadPart * 1000.0;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    cout << "=== Windows IPC Benchmark ("
         << ITERATIONS << " iterations, "
         << BUFFER_SIZE << " bytes) ===" << endl;
    cout << "Average pipe round-trip time : "
         << totalTime / ITERATIONS << " ms" << endl;

    return 0;
}