#include <windows.h>
#include <iostream>
using namespace std;

const int ITERATIONS = 20;

int main() {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    double total = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {
        STARTUPINFOA si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);
        char cmd[] = "C:\\Windows\\System32\\cmd.exe /c exit";

        QueryPerformanceCounter(&start);
        BOOL ok = CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        if (!ok) { cerr << "CreateProcess failed\n"; return 1; }
        WaitForSingleObject(pi.hProcess, INFINITE);
        QueryPerformanceCounter(&end);

        total += (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    cout << "RESULT:" << (total / ITERATIONS) * 1000 << endl;
    return 0;
}
