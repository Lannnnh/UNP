#include "../rw.h"

/*
    自旋锁：获取锁之前一直处于忙等(自旋)阻塞状态。
    适用情况：锁被持有的时间短，线程不希望在重新调度上花费太多的成本。

    屏障时用户协调多个线程并行工作的同步机制。
*/

#define NTHR 8 
#define NUMNUM 8000000L
#define TNUM (NUMNUM/NTHR)

long nums[NUMNUM];
long snums[NUMNUM];

int main()
{
    unsigned long i;
    struct timeval start, end;
    long long startusec, endusec;
    double elapsed;
    int err;
    pthread_t tid;

    srandom(1);
    for (i = 0; i < NTHR; ++ i )
        nums[i] = random();
    
    
}