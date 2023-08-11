

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUF_SIZE 512

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command\n");
        exit(1);
    }
    char* args[MAXARG+1];
    int index = 0;
    for (int i=1; i<argc; ++i) {
        args[index++] = argv[i];
    }

    char buf[BUF_SIZE];
    char *p = buf;
    while (read(0, p, 1) == 1) {
        if ((*p) == '\n') {
            *p = 0;
            int pid;
            if ((pid = fork()) == 0) {
                // child
                // exec only one arg
                args[index] = buf;
                exec(argv[1], args);
                fprintf(2, "exec %s failed\n", argv[1]);
                exit(0);
            }
            // parent
            wait(0);
            p = buf;
        } else {
            ++p;
        }
    }
    exit(0);
}
