#include <iostream>
#include <windows.h>
using namespace std;

const int   ITERATIONS  = 50;
const SIZE_T ALLOC_SIZE = 4096;

int main() {
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);

    double totalAlloc       = 0.0;
    double totalFirstAccess = 0.0;
    double totalFree        = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {

        /* 1. Allocation time — reserve + commit */
        QueryPerformanceCounter(&start);
        LPVOID ptr = VirtualAlloc(
            NULL,
            ALLOC_SIZE,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_READWRITE
        );
        QueryPerformanceCounter(&end);
        if (!ptr) {
            cerr << "VirtualAlloc failed. Error: " << GetLastError() << endl;
            return 1;
        }
        totalAlloc += static_cast<double>(end.QuadPart - start.QuadPart) /
                      frequency.QuadPart * 1000.0;

        /* 2. First access time — first write into the committed region */
        QueryPerformanceCounter(&start);
        memset(ptr, 'A', ALLOC_SIZE);
        QueryPerformanceCounter(&end);
        totalFirstAccess += static_cast<double>(end.QuadPart - start.QuadPart) /
                            frequency.QuadPart * 1000.0;

        /* 3. Free time */
        QueryPerformanceCounter(&start);
        VirtualFree(ptr, 0, MEM_RELEASE);
        QueryPerformanceCounter(&end);
        totalFree += static_cast<double>(end.QuadPart - start.QuadPart) /
                     frequency.QuadPart * 1000.0;
    }

    cout << "=== Windows Memory Management Benchmark ("
         << ITERATIONS << " iterations, "
         << ALLOC_SIZE << " bytes) ===" << endl;
    cout << "Average VirtualAlloc() time  : " << totalAlloc       / ITERATIONS << " ms" << endl;
    cout << "Average first access time    : " << totalFirstAccess / ITERATIONS << " ms" << endl;
    cout << "Average VirtualFree() time   : " << totalFree        / ITERATIONS << " ms" << endl;

    return 0;
}