#include "../rw.h"

static void recvfrom_int(int);
static int count;

void dg_echo(int sockfd, struct sockaddr* pcliaddr, socklen_t clilen)
{
    socklen_t len;
    char mesg[MAXLINE];

    signal(SIGINT, recvfrom_int);

    for ( ; ; )
    {
        len = clilen;
        recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

        count++;
    }
}

static void recvfrom_int(int signo)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    
    dg_echo(sockfd, (struct sockaddr*) &cliaddr, sizeof(cliaddr));
}