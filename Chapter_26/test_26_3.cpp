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
    str_echo((long) (arg));
    close((long) arg);
    return NULL;
}

/* 
给新线程传递参数

1. 传入引用值作为参数，connfd变量在每次调用accept的时候都会被覆写，多线程不是同步的访问一个共享变量
pthread_create(&tid, NULL, doit, &connfd);

static void* doit(void* arg)
{
    pthread_detach(pthread_self());
    str_echo(*(int*) (arg));
    close(*(int*) arg);
    return NULL;
}

解决方法
2. 调用malloc分配一个整数变量的内存空间，用于存放有待accept返回的已连接描述符，每个线程都有各自的已连接描述符版本。
int* iptr;
iptr = malloc(sizeof(int));
*iptr = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
pthread_create(&tid, NULL, doit, iptr);

static void* doit(void* arg)
{
    int connfd;

    connfd = *((int*) arg);
    free(arg);

    pthread_detach(pthread_self());
    str_echo(connfd);
    close(connfd);
    return NULL;
}
*/



