#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define ITERATIONS 20

int main() {
    struct timespec start, end;
    double total = 0.0;

    for (int i = 0; i < ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        pid_t pid = fork();
        if (pid == 0) {
            execl("/bin/true", "true", NULL);
            _exit(1);
        } else if (pid < 0) {
            perror("fork");
            return 1;
        }

        int status;
        waitpid(pid, &status, 0);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) * 1000.0
                       + (end.tv_nsec - start.tv_nsec) / 1e6;
        total += elapsed;
    }

    printf("RESULT:%.4f\n", total / ITERATIONS);
    return 0;
}
