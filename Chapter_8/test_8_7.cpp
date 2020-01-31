#include "../rw.h"

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen)
{
    int n;
    char sendLine[MAXLINE], recvLine[MAXLINE];

    while (fgets(sendLine, MAXLINE, fp) != NULL)
    {
        sendto(sockfd, sendLine, MAXLINE, 0, pservaddr, servlen);

        n = recvfrom(sockfd, recvLine, MAXLINE, 0, NULL, NULL);

        recvLine[n] = 0;
        fputs(recvLine, stdout);
    }
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);

    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}