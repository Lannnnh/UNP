#include "../rw.h"

// 用select来实现对fd的可读定时器
int readable_timeo(int fd, int sec)
{
    fd_set rset;
    struct timeval tv;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    tv.tv_sec = sec;
    tv.tv_usec = 0;
    // > 0表示文件描述符可读
    return select(fd+1, &rset, NULL, NULL, &tv);
}

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen)
{
    int n;
    char sendLine[MAXLINE], recvLine[MAXLINE];

    while (fgets(sendLine, MAXLINE, fp) != NULL)
    {
        sendto(sockfd, sendLine, strlen(sendLine), 0, pservaddr, servlen);

        if (readable_timeo(sockfd, 5) == 0)
        {
            fprintf(stderr, "socket timeout\n");
        }
        else
        {
            n = recvfrom(sockfd, recvLine, MAXLINE, 0, NULL, NULL);
            recvLine[n] = 0;
            fputs(recvLine, stdout);
        }
        
    }
}