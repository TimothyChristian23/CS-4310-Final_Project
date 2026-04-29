#include <iostream>
#include <windows.h>

using namespace std;

const int ITERATIONS = 50;

int main() {
    LARGE_INTEGER frequency, start, end;
    double totalTime = 0.0;

    QueryPerformanceFrequency(&frequency);

    for (int i = 0; i < ITERATIONS; i++) {
        STARTUPINFOA si{};
        PROCESS_INFORMATION pi{};

        si.cb = sizeof(si);

        char command[] = "C:\\Windows\\System32\\cmd.exe /c exit";

        QueryPerformanceCounter(&start);

        BOOL success = CreateProcessA(
            NULL,
            command,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        );

        if (!success) {
            cerr << "CreateProcess failed" << endl;
            return 1;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        QueryPerformanceCounter(&end);

        double elapsed =
            static_cast<double>(end.QuadPart - start.QuadPart) /
            frequency.QuadPart;

        totalTime += elapsed;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    cout << "Average CreateProcess() time over "
         << ITERATIONS
         << " runs: "
         << (totalTime / ITERATIONS) * 1000
         << " ms" << endl;

    return 0;
}