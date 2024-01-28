#include "kernel/types.h"
#include "user/user.h"

#define panic(msg)\
do {\
    fprintf(2, "panicked at %s, line %d: %s.\n", __FILE__, __LINE__, msg);\
    exit(1);\
} while (0)

// #define stub() printf("stub: file %s, line %d\n", __FILE__, __LINE__)
#define stub()

const int READ_END = 0;
const int WRITE_END = 1;
int pipe_fd[2];   // pipe fd

void
sieve(void)
{
    // printf("pid = %d\n", getpid());
    close(pipe_fd[WRITE_END]);  // we must close write end or read() will block forever
    int p;
    int result = read(pipe_fd[READ_END], (void *)&p, sizeof(int));
    if (result < 0) {
        panic("Failed to read");
    }
    if (result == 0) {      // EOF
        exit(0);
    }
    printf("prime %d\n", p);

    stub();

    int arr[16] = { 0 };    // store n, where n % p != 0
    int cnt = 0;  // count of elem in arr
    int n;
    while (0 < read(pipe_fd[READ_END], (void *)&n, sizeof(int))) {
        if (0 == n % p) {
            continue;
        }
        arr[cnt++] = n;
    }
    close(pipe_fd[READ_END]);

    stub();

    int new_pipe_fd[2];
    if (pipe(new_pipe_fd) < 0) {
        panic("Failed to pipe");
    }
    int pid = fork();
    if (0 == pid) {
        // printf("pid = %d\n", getpid());
        // printf("new_pipe_fd: [0] = %d, [1] = %d\n", new_pipe_fd[0], new_pipe_fd[1]);
        pipe_fd[0] = new_pipe_fd[0];
        pipe_fd[1] = new_pipe_fd[1];
        sieve();
    } else if (0 < pid) {
        close(new_pipe_fd[READ_END]);
        for (int i = 0; i < cnt; i++) {
            write(new_pipe_fd[WRITE_END], (const void *)&arr[i], sizeof(int));
        }
        close(new_pipe_fd[WRITE_END]);
        wait((int *)0);
    } else {
        panic("prime: Failed to fork");
    }

    exit(0);
}

int
main(int argc, char *argv[])
{
    pipe(pipe_fd);
    int pid = fork();
    if (0 == pid) {
        sieve();
    } else if (0 < pid) {
        close(pipe_fd[READ_END]);
        for (int i = 2; i <= 35; i++) {
            write(pipe_fd[WRITE_END], (const void *)&i, sizeof(int));
        }
        close(pipe_fd[WRITE_END]);  // reader will see EOF(?)
        wait((int *)0);
    } else {
        panic("prime: Failed to fork");
    }

    exit(0);
}
