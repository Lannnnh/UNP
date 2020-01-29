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
            if ((n == read(sockfd, buf, MAXLINE) == 0))
            {
                if (stdineof == 1) return ; // 正常结束
                else err_sys("terminate prematurely");
            }
            write(fileno(stdout), buf, n);
        }

        // fp是可读的
        if (FD_ISSET(fileno(fp), &rset))
        {
            if ((n == read(fileno(fp), buf, MAXLINE) == 0))
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