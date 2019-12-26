#include "../rw.h"

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
        writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR) goto again;
}

int main(int argc, char* argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);

    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    for ( ; ; )
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
        if ((childpid = fork()) == 0) 
        {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }
}