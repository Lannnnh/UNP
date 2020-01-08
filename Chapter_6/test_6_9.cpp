#include "../rw.h"

void str_cli(FILE* fp, int sockfd)
{
    int maxfdp1;
    fd_set rset;
    char sendLine[MAXLINE], recvLine[MAXLINE];

    FD_ZERO(&rset);
    for ( ; ; )
    {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = std::max(fileno(fp), sockfd) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset))
        {
            readline(sockfd, recvLine, MAXLINE);
            fputs(recvLine, stdout);
        }

        if (FD_ISSET(fileno(fp), &rset))
        {

            if (fgets(sendLine, MAXLINE, fp) == NULL)
                return ;
            writen(sockfd, sendLine, strlen(sendLine));
        }

    }
}