#include "../rw.h"

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

int main(int agrc, char* agrv[])
{
    int i, sockfd[5];
    struct sockaddr_in servaddr;

    for (i = 0; i < 5; ++ i )
    {
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(INADDR_ANY);
        inet_pton(AF_INET, agrv[1], &servaddr.sin_addr);

        connect(sockfd[i], (struct sockaddr*) &servaddr, sizeof(servaddr));
    }

    str_cli(stdin, sockfd[0]);
    exit(0);
}