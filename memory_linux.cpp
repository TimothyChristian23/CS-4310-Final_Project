#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <time.h>
using namespace std;

const int ITERATIONS  = 50;
const int ALLOC_SIZE  = 4096;

int main() {
    struct timespec start, end;

    double totalAlloc       = 0.0;
    double totalFirstAccess = 0.0;
    double totalFree        = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {

        /* 1. Allocation time — mmap() returns immediately, no pages loaded yet */
        clock_gettime(CLOCK_MONOTONIC, &start);
        void* ptr = mmap(
            NULL,
            ALLOC_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0
        );
        clock_gettime(CLOCK_MONOTONIC, &end);
        if (ptr == MAP_FAILED) { perror("mmap"); return 1; }
        totalAlloc += ((end.tv_sec  - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;

        /* 2. First access time — triggers page fault, kernel loads the page */
        clock_gettime(CLOCK_MONOTONIC, &start);
        memset(ptr, 'A', ALLOC_SIZE);
        clock_gettime(CLOCK_MONOTONIC, &end);
        totalFirstAccess += ((end.tv_sec  - start.tv_sec) +
                             (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;

        /* 3. Free time */
        clock_gettime(CLOCK_MONOTONIC, &start);
        munmap(ptr, ALLOC_SIZE);
        clock_gettime(CLOCK_MONOTONIC, &end);
        totalFree += ((end.tv_sec  - start.tv_sec) +
                      (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;
    }

    cout << "=== Linux Memory Management Benchmark ("
         << ITERATIONS << " iterations, "
         << ALLOC_SIZE << " bytes) ===" << endl;
    cout << "Average mmap() time        : " << totalAlloc       / ITERATIONS << " ms" << endl;
    cout << "Average first access time  : " << totalFirstAccess / ITERATIONS << " ms" << endl;
    cout << "Average munmap() time      : " << totalFree        / ITERATIONS << " ms" << endl;

    return 0;
}