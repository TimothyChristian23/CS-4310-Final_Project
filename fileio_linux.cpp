#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
using namespace std;

const int ITERATIONS  = 50;
const int BUFFER_SIZE = 4096;

int main() {
    struct timespec start, end;

    char writeBuffer[BUFFER_SIZE];
    char readBuffer[BUFFER_SIZE];
    memset(writeBuffer, 'A', BUFFER_SIZE);

    double totalOpen  = 0.0;
    double totalWrite = 0.0;
    double totalRead  = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {

        clock_gettime(CLOCK_MONOTONIC, &start);
        int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
        clock_gettime(CLOCK_MONOTONIC, &end);
        if (fd < 0) { perror("open"); return 1; }
        totalOpen += ((end.tv_sec  - start.tv_sec) +
                      (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;

        clock_gettime(CLOCK_MONOTONIC, &start);
        write(fd, writeBuffer, BUFFER_SIZE);
        clock_gettime(CLOCK_MONOTONIC, &end);
        totalWrite += ((end.tv_sec  - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;

        lseek(fd, 0, SEEK_SET);

        clock_gettime(CLOCK_MONOTONIC, &start);
        read(fd, readBuffer, BUFFER_SIZE);
        clock_gettime(CLOCK_MONOTONIC, &end);
        totalRead += ((end.tv_sec  - start.tv_sec) +
                      (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;

        close(fd);
    }

    unlink("test.txt");

    cout << "=== Linux File I/O Benchmark ("
         << ITERATIONS << " iterations, "
         << BUFFER_SIZE << " bytes) ===" << endl;
    cout << "Average open() time  : " << totalOpen  / ITERATIONS << " ms" << endl;
    cout << "Average write() time : " << totalWrite / ITERATIONS << " ms" << endl;
    cout << "Average read() time  : " << totalRead  / ITERATIONS << " ms" << endl;

    return 0;
}