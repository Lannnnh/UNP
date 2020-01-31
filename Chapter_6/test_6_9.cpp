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
        // 一直阻塞等待,直到有一个描述符准备好I/O时才返回。 返回值表示所有描述符集的已就绪的总位数。
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset))
        {
            // 1. 0 接收到FIN信号 2. -1 接收到RST信号 3. >0 接收到数据
            if ((readline(sockfd, recvLine, MAXLINE)) == 0)
                err_sys("server terminated prematurely");
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