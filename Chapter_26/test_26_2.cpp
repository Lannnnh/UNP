#include "../rw.h"

void* copyto(void *);

static int sockfd;
static FILE* fp;

void str_cli(FILE* fp_arg, int sockfd_arg)
{
    char recvLine[MAXLINE];
    pthread_t tid;

    sockfd = sockfd_arg;
    fp = fp_arg;

    pthread_create(&tid, NULL, copyto, NULL);

    while (readline(sockfd, recvLine, MAXLINE) > 0)
        fputs(recvLine, stdout);
}

void* copyto(void* arg)
{
    char sendLine[MAXLINE];

    while (fgets(sendLine, MAXLINE, fp) != NULL)
        write(sockfd, sendLine, strlen(sendLine));
    
    shutdown(sockfd, SHUT_WR);

    return NULL;
}

int main(int argc, char* argv[])
{
    int i, sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);
    exit(0);
}