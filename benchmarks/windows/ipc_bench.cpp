#include <windows.h>
#include <iostream>
using namespace std;

const int ITERATIONS = 1000;
const int MSG_SIZE = 1024;

int main() {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    double total = 0.0;

    char sendBuf[MSG_SIZE], recvBuf[MSG_SIZE];
    memset(sendBuf, 'X', MSG_SIZE);

    for (int i = 0; i < ITERATIONS; i++) {
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;

        HANDLE hRead, hWrite;
        if (!CreatePipe(&hRead, &hWrite, &sa, MSG_SIZE * 2)) {
            cerr << "CreatePipe failed\n";
            return 1;
        }

        QueryPerformanceCounter(&start);

        DWORD n;
        WriteFile(hWrite, sendBuf, MSG_SIZE, &n, NULL);
        ReadFile(hRead, recvBuf, MSG_SIZE, &n, NULL);

        QueryPerformanceCounter(&end);
        total += (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;

        CloseHandle(hRead);
        CloseHandle(hWrite);
    }

    cout << "RESULT:" << (total / ITERATIONS) * 1000 << endl;
    return 0;
}
