#include "../rw.h"

#define NDG 2000
#define DGLEN 1400

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen)
{
    int i;
    char sendLine[MAXLINE];

    for (int i = 0; i < NDG; ++ i )
    {
        sendto(sockfd, sendLine, DGLEN, 0, pservaddr, servlen);
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

    dg_cli(stdin, sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    exit(0);
}