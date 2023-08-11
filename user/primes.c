
#include "kernel/types.h"
#include "user/user.h"

void child_proc(int* p) {
    // read first num from pipe and print it
    int num;
    int ret;
    if ((ret = read(p[0], &num, sizeof(int))) != sizeof(int)) {
         
		exit(0);
    }
    
    printf("prime %d\n", num);

    // feed num into next pipe
    int p1[2];
    if (pipe(p1) < 0) {
        fprintf(2, "pipe p1 err\n");
        exit(1);
    }
    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "fork err\n");
        exit(1);
    } else if (pid == 0) {
        close(p[0]);
        close(p1[1]); // close pipe no need, otherwise read block
        // child
        child_proc(p1);
    } else {
        // parent
        close(p1[0]);
        int num1;
        int ret;
        while ((ret=read(p[0], &num1, sizeof(int))) == sizeof(int)) {
            if (num1 % num != 0) {
                if (write(p1[1], &num1, sizeof(int)) != sizeof(int)) {
                    printf(2, "write err\n");
                    exit(1); //退出 
                }
            }
        }
        close(p1[1]);
        close(p[0]);
        wait(0);
        exit(0);
    }
}

int main(int argc, char* argv[]) 
{
    int p[2];
    if (pipe(p) < 0) {
        fprintf(2, "pipe err\n");
        exit(1);
    }
    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "fork err\n");
        exit(1);
    } else if (pid == 0)  {//子进程 
        // child
        close(p[1]);
        child_proc(p);
    } else {
        // parent feed num into pipe
        close(p[0]);
        for (int i=2; i<=35; ++i) {
            if (write(p[1], &i, sizeof(int)) != sizeof(int)) {
                fprintf(2, "write err\n");
                exit(1);
            }
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
    exit(0);
}   
