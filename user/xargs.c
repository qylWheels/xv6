#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define panic(msg)\
do {\
    fprintf(2, "panicked at %s, line %d: %s.\n", __FILE__, __LINE__, msg);\
    exit(1);\
} while (0)

#define stub() printf("stub: file %s, line %d\n", __FILE__, __LINE__)
// #define stub()

int
main(int argc, char *argv[])
{
    char *args[MAXARG] = { 0 };
    // copy args in argv[] to args[]
    for (int i = 0; i < argc; i++) {
        args[i] = argv[i];
    }

    // read lines and execute
    #define BUFSZ 1024
    char line[BUFSZ];
    int p = 0;
    while (read(0, line + p, 1) > 0) {
        if (line[p] != '\n' && line[p] != '\r') {
            p++;
            continue;
        }

        line[p] = '\0';
        args[argc] = line;

        int pid = fork();
        if (pid == 0) {
            exec(args[1], args + 1);
            panic("Failed to exec");
        } else if (pid > 0) {
            wait((int *)0);
            p = 0;
        } else {
            panic("Failed to fork");
        }
    }

    exit(0);
}
