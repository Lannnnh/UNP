#include "../rw.h"

void str_cli(FILE* fp, int sockfd)
{
    char sendLine[MAXLINE], receiveLine[MAXLINE];
    while (fgets(sendLine, MAXLINE, fp) != NULL)
    {
        writen(sockfd, sendLine, strlen(sendLine));
        readline(sockfd, receiveLine, MAXLINE);     
        fputs(receiveLine, stdout);
    }
}

int main(int agrc, char* agrv[])
{
    int i, sockfd[5];
    struct sockaddr_in servaddr;

    for (i = 0; i < 5; ++ i )
    {
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(5555);
        inet_pton(AF_INET, agrv[1], &servaddr.sin_addr);

        connect(sockfd[i], (struct sockaddr*) &servaddr, sizeof(servaddr));
    }

    str_cli(stdin, sockfd[0]);
    exit(0);
}