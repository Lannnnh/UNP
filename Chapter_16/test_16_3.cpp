#include "../rw.h"

void str_cli(FILE* fp, int sockfd)
{
    int maxfdp1, val, stdineof;
    ssize_t n, nwritten;
    fd_set rset, wset;
    char *toiptr, *tooptr, *friptr, *froptr;
    char to[MAXLINE], fr[MAXLINE];
    
    val = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, val | O_NONBLOCK);

    val = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

    val = fcntl(STDOUT_FILENO, F_GETFL, 0);
    fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;
    maxfdp1 = std::max(std::max(STDIN_FILENO, STDOUT_FILENO), sockfd)+1;

    for ( ; ; )
    {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        // 标准输入还没有结束，打开监听
        if (stdineof == 0 && toiptr < &to[MAXLINE])
            FD_SET(STDIN_FILENO, &rset);
        if (friptr < &fr[MAXLINE])
            FD_SET(STDOUT_FILENO, &rset);
        if (tooptr != toiptr) FD_SET(STDIN_FILENO, &wset);
        if (friptr != froptr) FD_SET(STDOUT_FILENO, &wset);

        select(maxfdp1, &rset, &wset, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &rset))
        {
            if ((n = read(STDIN_FILENO, toiptr, &to[MAXLINE]-toiptr)) < 0)
            {
                if (errno == EWOULDBLOCK) err_sys("read error on stdin");
            }
            else if (n == 0)
            {
                fprintf(stderr, "EOF on stdin\n");
                stdineof = 1;
                if (tooptr == toiptr) shutdown(sockfd, SHUT_WR);
            }
            else
            {
                fprintf(stderr, "read %d bytes from stdin\n", n);
                toiptr += n;
                FD_SET(sockfd, &wset);
            }
        }

        if (FD_ISSET(sockfd, &rset))
        {
            if ((n = read(sockfd, friptr, &fr[MAXLINE]-friptr)) < 0)
            {
                if (errno == EWOULDBLOCK) err_sys("read error on socket");
            }
            else if (n == 0)
            {
                fprintf(stderr, "EOF on socket\n");
                if (stdineof) return ;  // 接收到数据量为0，并且标准输入停止，正常结束
                else err_sys("str_cli: server terminated prematurely");
            }
            else
            {
                fprintf(stderr, "read %d bytes from socket\n", n);
                friptr += n;
                FD_SET(STDOUT_FILENO, &wset);
            }
        }

        if (FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr-froptr) > 0))
        {
            if ((nwritten = write(STDOUT_FILENO, froptr, n)) < 0)
            {
                if (errno == EWOULDBLOCK) err_sys("write error on stdout");
            }
            else
            {
                fprintf(stderr, "wrote %d bytes to stdout\n", nwritten);
                froptr += nwritten;
                if (froptr == friptr) froptr = friptr = fr;
            }
        }

        if (FD_ISSET(sockfd, &wset) && ((n = toiptr-tooptr) > 0))
        {
            if ((nwritten = write(sockfd, tooptr, n)) < 0)
            {
                if (errno == EWOULDBLOCK) err_sys("write error on socket");
            }
            else
            {
                fprintf(stderr, "wrote %d bytes to socket\n", nwritten);
                tooptr += nwritten;
                if (tooptr == toiptr) 
                {
                    tooptr = toiptr = to;
                    if (stdineof)
                        shutdown(sockfd, SHUT_WR);
                }
            }
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