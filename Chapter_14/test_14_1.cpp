#include "../rw.h"

static void connect_alarm(int);

int connect_timeo(int sockfd, const struct sockaddr* saptr, socklen_t salen, int nsec)
{
    Sigfunc* sigfunc;
    int n;
    // signal函数的返回值就是signal()函数上一次的行为
    sigfunc = signal(SIGALRM, connect_alarm);
    // SIGALRM信号默认处理终止进程
    if (alarm(nsec) != 0)
        err_sys("connect_timeo: alarm was already set");
    
    if ((n = connect(sockfd, saptr, salen)) < 0)
    {
        close(sockfd);
        if (errno == EINTR) errno = ETIMEDOUT;
    }
    // alarm设置时间为0，那么设置的alarm被取消，并返回剩余时间
    alarm(0);
    signal(SIGALRM, sigfunc);

    return n;
}

static void connect_alarm(int signo)
{
    return ; // 仅仅只是打断connect()
}

int main(int argc, char* argv[])
{
    
}