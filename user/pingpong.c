
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);

    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "fork err");
        exit(1);
    } else if (pid == 0) {
        // child
        close(p1[0]);
        close(p2[1]);
        char buf[1];
        if (read(p2[0], buf, 1) > 0) {
            fprintf(1, "%d: received ping\n", getpid());
        }
        write(p1[1], buf, 1);
        close(p1[1]);
        exit(0);
    } else {
        // parent
        close(p1[1]);
        close(p2[0]);
        write(p2[1], "h", 1);
        close(p2[1]);
        char buf[1];
        if(read(p1[0], buf, 1) > 0){
            fprintf(1, "%d: received pong\n", getpid());
        }
        wait(0);
        exit(0);
    }

}
