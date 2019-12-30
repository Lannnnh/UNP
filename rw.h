#ifndef _RW_H_
#define _RW_H_

#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 1024
#define LISTENQ 1024

void err_sys(const char* str) 
{ 
    fprintf(stderr, "%s\n", str); 
    exit(1); 
}

ssize_t readn(int fd, void* vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char* ptr;

    ptr = (char*) vptr;
    nleft = n;

    while (nleft > 0)
    {
        // 需要处理nread < 0的情况
        nread = read(fd, ptr, nleft);
        nleft -= nread;
        ptr += nread;
    }

    return n-nleft;
}

ssize_t writen(int fd, void* vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    char* ptr;

    ptr = (char*) vptr;
    nleft = n;

    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft);
        nleft -= nwritten;
        ptr += nwritten;
    }

    return n-nleft;
}

ssize_t readline(int fd, void* vptr, size_t maxlen)
{
    size_t n, rc;
    char c, *ptr;

    ptr = (char*) vptr;
    for (n = 1; n < maxlen; ++ n )
    {
    again: 
        if ((rc = read(fd, &c, 1)) == 1)
        {
            *ptr++ = c;
            if (c == '\n') break;
        }
        else if (rc == 0)
        {
            *ptr = 0;
            return n-1;
        }
        else
        {
            // 需要处理errno=EINTR
            if (errno == EINTR) goto again;
            return -1;
        }
    }
    *ptr = 0;
    return n;
}

#endif // end _RW_H_