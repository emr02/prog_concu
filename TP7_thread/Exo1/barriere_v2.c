#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

struct structArg
{
    int i;
    pthread_barrier_t* barrier;
};

void *task1(void *arg)
{
    struct structArg *args = arg;
    
    // Simulate some work
    int sleep_time = rand() % 3 + 1;
    printf("Thread %d: Working for %d seconds...\n", args->i, sleep_time);
    sleep(sleep_time);
    
    printf("Thread %d: Arrived at rendez-vous point\n", args->i);
    
    // Wait at the barrier for all threads to arrive
    pthread_barrier_wait(args->barrier);
    
    // After rendez-vous
    printf("Thread %d: Continuing execution after rendez-vous\n", args->i);
    
    // More work after synchronization
    sleep_time = rand() % 2 + 1;
    sleep(sleep_time);
    printf("Thread %d: Finished execution\n", args->i);
    
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        exit(-1);
    }

    int N = atoi(argv[1]);

    // Initialize random seed
    srand(time(NULL));

    pthread_t thread[N];
    pthread_barrier_t barrier;
    
    // Initialize barrier for N threads
    pthread_barrier_init(&barrier, NULL, N);

    struct structArg args[N];
    
    for (int i = 0; i < N; i++) {
        args[i].i = i;
        args[i].barrier = &barrier;
        pthread_create(&thread[i], NULL, task1, &args[i]);
    }
    
    for (int i = 0; i < N; i++)
        pthread_join(thread[i], NULL);

    pthread_barrier_destroy(&barrier);

    return 0;
}