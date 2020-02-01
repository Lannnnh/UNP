#include "../rw.h"

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen)
{
    int n;
    char sendLine[MAXLINE], recvLine[MAXLINE];
    socklen_t len; 
    struct sockaddr* preply_addr;

    preply_addr = (struct sockaddr*) malloc(servlen);

    while (fgets(sendLine, MAXLINE, fp) != NULL)
    {
        sendto(sockfd, sendLine, strlen(sendLine), 0, pservaddr, servlen);

        len = servlen;
        n = recvfrom(sockfd, recvLine, MAXLINE, 0, preply_addr, &len);

        if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0)
        {
            printf("reply from %s (igonored)\n", sock_ntop(preply_addr, len));
            continue;
        }

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
    servaddr.sin_port = htons(7);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli(stdin, sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    exit(0);
}