#include "../rw.h"

#define NLOOP 5000

int counter;

void* doit(void*);

int main(int argc, char* argv[])
{
    pthread_t tidA, tidB;

    pthread_create(&tidA, NULL, &doit, NULL);
    pthread_create(&tidB, NULL, &doit, NULL);

    pthread_join(tidA, NULL);
    pthread_join(tidB, NULL);

    exit(0);
}

void* doit(void* arg)
{
    int i, val;

    for (i = 0; i < NLOOP; i++ )
    {
        val = counter;
        printf("%d: %d\n", pthread_self(), val+1);
        counter = val+1;
    }
    return NULL;
}