#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define ITERATIONS 200
#define BUF_SIZE 1024

int main() {
    struct timespec start, end;
    double total = 0.0;
    char writeBuf[BUF_SIZE], readBuf[BUF_SIZE];
    memset(writeBuf, 'A', BUF_SIZE);

    for (int i = 0; i < ITERATIONS; i++) {
        char tmpfile[] = "/tmp/bench_XXXXXX";
        int fd = mkstemp(tmpfile);
        if (fd < 0) { perror("mkstemp"); return 1; }
        close(fd);
        unlink(tmpfile);

        clock_gettime(CLOCK_MONOTONIC, &start);

        fd = open(tmpfile, O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); return 1; }

        write(fd, writeBuf, BUF_SIZE);
        lseek(fd, 0, SEEK_SET);
        read(fd, readBuf, BUF_SIZE);
        close(fd);
        unlink(tmpfile);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) * 1000.0
                       + (end.tv_nsec - start.tv_nsec) / 1e6;
        total += elapsed;
    }

    printf("RESULT:%.4f\n", total / ITERATIONS);
    return 0;
}
