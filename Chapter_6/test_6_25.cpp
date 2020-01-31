#include "../rw.h"

int main(int argc, char* argv[])
{
    int i, maxi, listenfd, sockfd, connfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct pollfd client[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for (int i = 1; i < MAXLINE; ++ i ) client[i].fd = -1;
    maxi = 0;

    for ( ; ; )
    {
        // timeout为-1永久等待
        nready = poll(client, maxi+1, -1);

        if (client[0].revents & POLLRDNORM)
        {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);

            for (i = 1; i < MAXLINE; ++ i )
            {
                if (client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    break;
                }
            }

            if (i == MAXLINE) err_sys("too many client");

            client[i].events = POLLRDNORM;
            if (i > maxi) maxi = i;

            if (--nready <= 0) continue;
        }

        for (i = 1; i <= maxi; ++ i )
        {
            if ((sockfd = client[i].fd) < 0) continue;

            if (client[i].revents & (POLLRDNORM | POLLERR))
            {
                if ((n = read(sockfd, buf, MAXLINE)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else err_sys("read error");
                }
                else if (n == 0)
                {
                    close(sockfd);
                    client[i].fd = -1;
                }
                else write(sockfd, buf, n); 

                if (--nready == 0) break;
            }
        }
    }
}