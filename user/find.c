
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 从路径获取文件名
char* fmtname(char *path)
{
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
}

// 递归的对路径path寻找filename
void find_aux(char* path, char* filename) {
    char buf[512];
    int fd;
    char* p;
    struct stat st;
    struct dirent de;
    char* cur_name = fmtname(path);
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find_aux: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find_aux: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch (st.type) {
    // 如果是文件，则对比名称
    case T_FILE:
        if (strcmp(cur_name, filename) == 0) {
            printf("%s\n", path);
        }
        break;
    // 如果是目录则深入寻找
    case T_DIR:
        // do not dive into .. or .
        if (strcmp(".", cur_name) == 0 || strcmp("..", cur_name) == 0) {
            return;
        }
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find_aux: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            memmove(p, &de.name, DIRSIZ);
            find_aux(buf, filename);
        }
    }
    close(fd);
}

int main(int argc, char* argv[]) 
{
    if (argc != 3) {
        fprintf(2, "Usage: find dir filename\n");
        exit(1);
    }
    // handle . and ..
    if (strcmp(".", argv[1]) == 0 || strcmp("..", argv[1]) == 0) {
        int fd;
        char buf[30];
        strcpy(buf, argv[1]);
        int len = strlen(buf);
        buf[len++] = '/';
        if ((fd = open(argv[1], 0)) < 0) {
            fprintf(2, "main: cannot open %s\n", argv[1]);
            exit(1);
        }
        struct dirent de;
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            // avoid . and ..
            if(strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0)
                continue;
            // 读的时候会有个空路径，需要排除
            if(de.inum == 0)
                continue;
            memmove(buf+len, de.name, DIRSIZ);
            find_aux(buf, argv[2]);
        }
        close(fd);
    } else {
        find_aux(argv[1], argv[2]);
    }
    exit(0);
}
