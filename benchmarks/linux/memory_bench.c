#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

#define ITERATIONS 1000
#define ALLOC_SIZE (1024 * 1024)

int main() {
    struct timespec start, end;
    double total = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        void* mem = mmap(NULL, ALLOC_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (mem == MAP_FAILED) { perror("mmap"); return 1; }

        memset(mem, 0, ALLOC_SIZE);
        munmap(mem, ALLOC_SIZE);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) * 1000.0
                       + (end.tv_nsec - start.tv_nsec) / 1e6;
        total += elapsed;
    }

    printf("RESULT:%.4f\n", total / ITERATIONS);
    return 0;
}
