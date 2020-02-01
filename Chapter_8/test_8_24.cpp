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

int main(int argc, char* argv[])
{
    int listenfd, connfd, sockfd, udpfd, nready, maxfdp1;
    struct sockaddr_in servaddr, cliaddr;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    void sig_chld(int);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    udpfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5555);

    bind(udpfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    signal(SIGCHLD, sig_chld);

    FD_ZERO(&rset);
    maxfdp1 = std::max(listenfd, udpfd)+1;

    for ( ; ; )
    {
        FD_SET(udpfd, &rset);
        FD_SET(listenfd, &rset);
        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR) continue;
            else err_sys("select error");
        }

        if (FD_ISSET(listenfd, &rset))
        {
            len = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &len);

            if ((childpid = fork()) == 0)
            {
                close(listenfd);
                str_echo(connfd);
                exit(0);
            }
        }

        if (FD_ISSET(udpfd, &rset))
        {
            len = sizeof(cliaddr);
            n = recvfrom(udpfd, mesg, MAXLINE, 0, (struct sockaddr*) &cliaddr, &len);
            sendto(udpfd, mesg, n, 0, (struct sockaddr*) &cliaddr, len);
        }
    }
}