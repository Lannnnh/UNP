#include "../rw.h"

int main(int argc, char* argv[])
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set rset, allset;
    char buf[MAXLINE];
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(5555);
    servaddr.sin_family = AF_INET;

    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; ++ i ) client[i] = -1;

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    for ( ; ; )
    {
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset))
        {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);

            for (i = 0; i < FD_SETSIZE; ++ i ) 
                if (client[i] < 0)
                {
                    client[i] = connfd;
                    break;
                }
            
            if (i == FD_SETSIZE) err_sys("too many client");

            FD_SET(connfd, &allset);
            if (connfd > maxfd) maxfd = connfd;
            if (i > maxi) maxi = i;
            if (--nready <= 0) continue;
        }

        for (i = 0; i < FD_SETSIZE; ++ i )
        {
            if ((sockfd = client[i]) < 0) continue;

            if (FD_ISSET(sockfd, &rset))
            {
                if ((n = read(sockfd, buf, MAXLINE)) == 0)
                {
                    close(sockfd);
                    client[i] = -1;
                    FD_CLR(sockfd, &allset);
                }
                else 
                {
                    write(sockfd, buf, n);
                    printf("wrote %d biytes to sockfd\n", n);
                }

                if (--nready <= 0) break;
            }
        }
    }
}