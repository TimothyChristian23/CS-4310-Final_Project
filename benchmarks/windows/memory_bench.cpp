#include <windows.h>
#include <iostream>
using namespace std;

const int ITERATIONS = 1000;
const SIZE_T ALLOC_SIZE = 1024 * 1024; // 1 MB

int main() {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    double total = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {
        QueryPerformanceCounter(&start);

        void* mem = VirtualAlloc(NULL, ALLOC_SIZE,
            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!mem) { cerr << "VirtualAlloc failed\n"; return 1; }

        memset(mem, 0, ALLOC_SIZE);
        VirtualFree(mem, 0, MEM_RELEASE);

        QueryPerformanceCounter(&end);
        total += (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    }

    cout << "RESULT:" << (total / ITERATIONS) * 1000 << endl;
    return 0;
}
