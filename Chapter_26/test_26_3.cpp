#include "../rw.h"

static void* doit(void *);

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
    pthread_t tid;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    for ( ; ; )
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
        pthread_create(&tid, NULL, doit, (void*) connfd);
    }
}

static void* doit(void* arg)
{
    pthread_detach(pthread_self());
    str_echo((int) arg);
    close((int) arg);
    return NULL;
}


