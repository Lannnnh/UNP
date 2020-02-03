#include "../rw.h"

void str_cli(FILE* fp, int sockfd)
{
    pid_t pid;
    char sendLine[MAXLINE], recvLine[MAXLINE];

    // 子进程：server->stdout
    if ((pid = fork()) == 0)
    {  
        while (readline(sockfd, recvLine, MAXLINE) > 0) 
            fputs(recvLine, stdout);

        kill(getppid(), SIGTERM);
        exit(0);
    }
    // 父进程：stdin->server
    while (fgets(sendLine, MAXLINE, fp) != NULL)
        write(sockfd, sendLine, strlen(sendLine));
    shutdown(sockfd, SHUT_WR);
    pause();
    return ;
}

int main(int argc, char* argv[])
{
    int i, sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0)
        printf("%s\n", "inet_pton fail");

    connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);
    exit(0);
}