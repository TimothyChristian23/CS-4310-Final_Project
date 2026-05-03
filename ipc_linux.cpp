#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
using namespace std;

const int ITERATIONS  = 50;
const int BUFFER_SIZE = 4096;

int main() {
    struct timespec start, end;

    /* parent_to_child[0] = read, parent_to_child[1] = write */
    /* child_to_parent[0] = read, child_to_parent[1] = write */
    int parent_to_child[2];
    int child_to_parent[2];

    if (pipe(parent_to_child) < 0 || pipe(child_to_parent) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* --- CHILD --- */
        close(parent_to_child[1]); /* child doesn't write to this */
        close(child_to_parent[0]); /* child doesn't read from this */

        char buf[BUFFER_SIZE];
        for (int i = 0; i < ITERATIONS; i++) {
            /* Read message from parent */
            read(parent_to_child[0], buf, BUFFER_SIZE);
            /* Echo it back to complete the round trip */
            write(child_to_parent[1], buf, BUFFER_SIZE);
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        _exit(0);

    } else {
        /* --- PARENT --- */
        close(parent_to_child[0]); /* parent doesn't read from this */
        close(child_to_parent[1]); /* parent doesn't write to this */

        char sendBuf[BUFFER_SIZE];
        char recvBuf[BUFFER_SIZE];
        memset(sendBuf, 'A', BUFFER_SIZE);

        double totalTime = 0.0;

        for (int i = 0; i < ITERATIONS; i++) {
            clock_gettime(CLOCK_MONOTONIC, &start);

            /* Send to child */
            write(parent_to_child[1], sendBuf, BUFFER_SIZE);
            /* Wait for child to echo back */
            read(child_to_parent[0], recvBuf, BUFFER_SIZE);

            clock_gettime(CLOCK_MONOTONIC, &end);

            totalTime += ((end.tv_sec  - start.tv_sec) +
                          (end.tv_nsec - start.tv_nsec) / 1e9) * 1000.0;
        }

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        waitpid(pid, NULL, 0);

        cout << "=== Linux IPC Benchmark ("
             << ITERATIONS << " iterations, "
             << BUFFER_SIZE << " bytes) ===" << endl;
        cout << "Average pipe round-trip time : "
             << totalTime / ITERATIONS << " ms" << endl;
    }

    return 0;
}