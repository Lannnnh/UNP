#include "../rw.h"

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
        writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR) goto again;
}

int main(int agrc, char* argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);
    // SIGCHLD信号，在一个进程终止或者停止时，将SIGCHLD信号发送给父进程，默认忽略该信号。若父进程想被告知子进程的该状态，就要捕捉该信号。
    signal(SIGCHLD, sig_chld);

    for ( ; ; )
    {
        clilen = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0)
        {
            if (errno == EINTR) 
                continue;
            else 
                err_sys("accpet error!");
        }
        // child process
        if ( (childpid = fork()) == 0)
        {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }
}