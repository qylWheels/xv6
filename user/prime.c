#include "kernel/types.h"
#include "user/user.h"

#define panic(msg)  \
do {    \
    fprintf(2, "panicked at %s, line %d: %s.\n", __FILE__, __LINE__, msg);   \
    exit(1);    \
} while (0)

const int READ_END = 0;
const int WRITE_END = 1;
int pipe_fd[2];   // pipe fd

void
sieve(void)
{
    printf("pid = %d\n", getpid());
    int p;
    if (read(pipe_fd[READ_END], (void *)&p, sizeof(int)) < 0) {
        panic("Failed to read");
    }
    printf("prime %d\n", p);

    int arr[64] = { 0 };    // store n, where n % p != 0
    int cnt = 0;  // count of elem in arr
    int n;
    close(pipe_fd[WRITE_END]);  // we must close write end or read() will block forever
    while (0 < read(pipe_fd[READ_END], (void *)&n, sizeof(int))) {
        if (0 == n % p) {
            continue;
        }
        arr[cnt++] = n;
    }
    close(pipe_fd[READ_END]);

    int new_pipe_fd[2];
    if (pipe(new_pipe_fd) < 0) {
        panic("Failed to pipe");
    }
    int pid = fork();
    if (0 == pid) {
        printf("pid = %d\n", getpid());
        printf("new_pipe_fd: [0] = %d, [1] = %d\n", new_pipe_fd[0], new_pipe_fd[1]);
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

// #include "kernel/types.h"
// #include "kernel/stat.h"
// #include "user/user.h"

// void new_proc(int p[2]){
//     printf("pid = %d\n", getpid());
// 	int prime;
// 	int flag;
// 	int n;
// 	close(p[1]);
// 	if(read(p[0], &prime, 4) != 4){
// 		fprintf(2, "child process failed to read\n");
// 		exit(1);
// 	}
// 	printf("prime %d\n", prime);

// 	flag = read(p[0], &n, 4);
// 	if(flag){
// 		int newp[2];
// 		pipe(newp);
// 		if (fork() == 0)
// 		{
// 			new_proc(newp);
// 		}else
// 		{
// 			close(newp[0]);
// 			if(n%prime)write(newp[1], &n, 4);
			
// 			while(read(p[0], &n, 4)){
// 				if(n%prime)write(newp[1], &n, 4);
// 			}
// 			close(p[0]);
// 			close(newp[1]);
// 			wait(0);
// 		}
// 	}
// 	exit(0);
// }

// int main(int argc, char const *argv[])
// {
// 	int p[2];
// 	pipe(p);
// 	if (fork() == 0)
// 	{
// 		new_proc(p);
// 	}else
// 	{
// 		close(p[0]);
// 		for(int i = 2; i <= 35; i++)
// 		{
// 			if (write(p[1], &i, 4) != 4)
// 			{
// 				fprintf(2, "first process failed to write %d into the pipe\n", i);
// 				exit(1);
// 			}
// 		}
// 		close(p[1]);
// 		wait(0);
// 		exit(0);
// 	}
// 	return 0;
// }
