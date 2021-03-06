#include <time.h>
#include "../rw.h"

int main(int argc, char* argv[])
{
    int connfd, listenfd;
    struct sockaddr_in serveraddr;
    char buf[MAXLINE];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("%s %d\n", "listenfd", listenfd);
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(13);

    bind(listenfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    printf("%s\n", "bind");
    listen(listenfd, LISTENQ);
    printf("%s\n", "listen");

    for ( ; ; ) 
    {
        connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
        printf("%s %d\n", "connfd", connfd);
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        write(connfd, buf, sizeof(buf));
        close(connfd);
    }
}