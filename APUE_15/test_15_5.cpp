#include "../rw.h"

int main(void)
{
    int n;
    int fd[2];
    pid_t pid;
    char line[MAXLINE];

    pipe(fd);

    if ((pid = fork()) > 0) // parent
    {
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    }
    else
    {
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
    }
    
    exit(0);
}