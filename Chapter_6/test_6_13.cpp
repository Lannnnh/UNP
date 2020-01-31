#include "../rw.h"

void str_cli(FILE* fp, int sockfd)
{
    int maxfdp1, stdineof;
    fd_set rset;
    char buf[MAXLINE];
    int n;

    stdineof = 0;
    FD_ZERO(&rset);

    for ( ; ; )
    {
        if (stdineof == 0) FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = std::max(fileno(fp), sockfd)+1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        // sockfd是可读的
        if (FD_ISSET(sockfd, &rset))
        {
            if ((n = read(sockfd, buf, MAXLINE)) == 0)
            {
                // stdineof代表stdin缓冲区是否读取结束
                if (stdineof == 1) return ; // 正常结束
                else err_sys("terminate prematurely");
            }
            write(fileno(stdout), buf, n);
        }

        // fp是可读的
        if (FD_ISSET(fileno(fp), &rset))
        {
            // 从fp读取不到数据(EOF)时，关闭fp的写，并且从FD_SET中删除
            if ((n = read(fileno(fp), buf, MAXLINE)) == 0)
            {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            write(sockfd, buf, n);
        }
    }
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