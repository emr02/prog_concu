#include <stdlib.h>
#include <stdio.h>
#include<unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 4

struct structArg
{
    pthread_mutex_t *mutex;
    int count;
};
void *task1(void *arg)
{
    struct structArg *args = arg;
    int counts = 0;
    for (int i = 0; i < 1000000; i++)
        counts += 1;
    pthread_mutex_lock(args->mutex);
    args->count += counts;
    pthread_mutex_unlock(args->mutex);
    return NULL;
}
int main(void)
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_t thread[N];
    struct structArg args;
    args.mutex = &mutex;
    args.count = 0;
    for (int i = 0; i < N; i++)
        pthread_create(&thread[i], NULL, task1, &args);
    for (int i = 0; i < N; i++)
        pthread_join(thread[i], NULL);
    pthread_mutex_destroy(&mutex);
    printf("Count %d\n", args.count);