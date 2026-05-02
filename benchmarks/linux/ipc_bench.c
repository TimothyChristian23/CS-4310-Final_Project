#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 1000
#define MSG_SIZE 1024

int main() {
    struct timespec start, end;
    double total = 0.0;
    char sendBuf[MSG_SIZE], recvBuf[MSG_SIZE];
    memset(sendBuf, 'X', MSG_SIZE);

    for (int i = 0; i < ITERATIONS; i++) {
        int pipefd[2];
        if (pipe(pipefd) < 0) { perror("pipe"); return 1; }

        clock_gettime(CLOCK_MONOTONIC, &start);

        write(pipefd[1], sendBuf, MSG_SIZE);
        read(pipefd[0], recvBuf, MSG_SIZE);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) * 1000.0
                       + (end.tv_nsec - start.tv_nsec) / 1e6;
        total += elapsed;

        close(pipefd[0]);
        close(pipefd[1]);
    }

    printf("RESULT:%.4f\n", total / ITERATIONS);
    return 0;
}
