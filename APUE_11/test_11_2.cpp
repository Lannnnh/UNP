#include "../rw.h"

pthread_t ntid;

void printtids(const char* s)
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();
    printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long) pid, (unsigned long) tid, (unsigned long) tid);
}

void* thr_fn(void* arg)
{
    printtids("new thread: ");
    return NULL;
}

int main(void)
{
    int err;

    err = pthread_create(&ntid, NULL, thr_fn, NULL);
    if (err != 0) err_sys("can't creat thread");
    printtids("main thread:");
    sleep(1);
    exit(0);
}