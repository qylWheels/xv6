#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

#define panic(msg)\
do {\
    fprintf(2, "panicked at %s, line %d: %s.\n", __FILE__, __LINE__, msg);\
    exit(1);\
} while (0)

// #define stub() printf("stub: file %s, line %d\n", __FILE__, __LINE__)
#define stub()

#define BUFSZ 1024
static char PATH[BUFSZ];
static char FILENAME[BUFSZ];

char *
strrchr(char *s, char c)
{
    if (!s) {
        panic("Nullptr\n");
    }

    uint len = strlen(s);
    for (char *p = s + len; p >= s; p--) {
        if (*p == c) {
            return p;
        }
    }
    return 0;
}

char *
strcat(char *dest, const char *src)
{
    char *p = dest + strlen(dest);
    while (*src != '\0') {
        *p++ = *src++;
    }
    *p = '\0';
    return dest;
}

void
find()
{
    int fd = open(PATH, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "Cannot open() %s\n", PATH);
        return;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        fprintf(2, "Cannot stat() %s\n", PATH);
        return;
    }

    stub();

    switch (st.type) {
    default: {
        panic("Invalid file type");
        break;
    }
    case T_DEVICE:
    case T_FILE: {
        stub();
        if (0 == strcmp(strrchr(PATH, '/') + 1, FILENAME)) {
            stub();
            printf("%s\n", PATH);
        }
        break;
    }
    case T_DIR: {
        struct dirent de;
        while (read(fd, &de, sizeof de) == sizeof de) {
            if (0 == strcmp(de.name, ".")
             || 0 == strcmp(de.name, "..")
             || de.inum == 0 /* Space for creating files and folders, must be ignored */)
            {
                continue;
            }
            stub();
            strcat(PATH, "/");
            strcat(PATH, de.name);
            stub();
            find();
            stub();
            *strrchr(PATH, '/') = '\0';
            stub();
        }
        break;
    }
    }

    close(fd);

    return;
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        panic("Usage: find <path> <filename>\n");
    }
    strcpy(PATH, argv[1]);
    strcpy(FILENAME, argv[2]);
    find();
    exit(0);
}
