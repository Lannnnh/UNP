#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "rw.h"

#define MAXLINE 1024

void str_cli(FILE* fp, int sockfd)
{
    char sendLine[MAXLINE], receiveLine[MAXLINE];
    while (fgets(sendLine, MAXLINE, fp) != NULL)
    {
        // printf("sendline: %s\n", sendLine);
        writen(sockfd, sendLine, strlen(sendLine));
        // printf("sendline: %s\n", sendLine);
        readline(sockfd, receiveLine, MAXLINE);     
        // printf("receiveline: %s\n", receiveLine);
        fputs(receiveLine, stdout);
    }
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    printf("connect ss: %d\n", sockfd);
    str_cli(stdin, sockfd);
    
    exit(0);
}