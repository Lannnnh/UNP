#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 24

int main(int argc, char* argv[])
{
    printf("%d\n", argc);
    printf("%s\n", argv[1]);

    int sockfd, n;
    char readline[MAXLINE+1];
    struct sockaddr_in serveraddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("%s\n", "sockfd fail");
    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(13);
    if (inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) < 0)
        printf("%s\n", "inet_pton fail");

    printf("%s %d\n", "inet_pton", (int) serveraddr.sin_addr.s_addr);

    if (connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0)
        printf("%s\n", "fail");

    printf("%s %d\n", "connected", sockfd);

    while ((n = read(sockfd, readline, MAXLINE)) > 0)
    {
        printf("%s\n", "read");
        readline[n] = 0;
        fputs(readline, stdout);
    }
    
    exit(0);
}