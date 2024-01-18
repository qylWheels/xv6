#include "kernel/types.h"
#include "user/user.h"

#define READ_END 0
#define WRITE_END 1

int
main(int argc, char *argv[])
{
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        fprintf(2, "Failed to pipe\n");
        exit(1);
    }

    char byte = '!';
    int pid = fork();
    if (0 == pid) {    // child
        read(pipe_fd[READ_END], &byte, 1);
        printf("%d: received ping\n", getpid());
        write(pipe_fd[WRITE_END], &byte, 1);
        close(pipe_fd[READ_END]);
        close(pipe_fd[WRITE_END]);
        exit(0);
    } else if (0 < pid) {    // parent
        write(pipe_fd[WRITE_END], &byte, 1);
        wait((int *)0);
        read(pipe_fd[READ_END], &byte, 1);
        printf("%d: received pong\n", getpid());
        close(pipe_fd[READ_END]);
        close(pipe_fd[WRITE_END]);
    } else {
        fprintf(2, "pingpong: Failed to fork\n");
        close(pipe_fd[READ_END]);
        close(pipe_fd[WRITE_END]);
        exit(1);
    }

    exit(0);
}
