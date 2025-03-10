#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

struct structArg
{
    int i;
    int N;
    sem_t *sem;
    pthread_mutex_t *mutex;
    int *count;  // Shared counter to track threads at rendez-vous point
};

void *task1(void *arg)
{
    struct structArg *args = arg;
    
    // Simulate some work
    int sleep_time = rand() % 3 + 1;
    printf("Thread %d: Working for %d seconds...\n", args->i, sleep_time);
    sleep(sleep_time);
    
    printf("Thread %d: Arrived at rendez-vous point\n", args->i);
    
    // Critical section: update counter and check if all threads have arrived
    pthread_mutex_lock(args->mutex);
    (*args->count)++;
    
    if (*args->count < args->N) {
        // Not all threads have arrived yet, wait
        printf("Thread %d: Waiting for others (%d/%d arrived)\n", 
               args->i, *args->count, args->N);
        pthread_mutex_unlock(args->mutex);
        
        // Wait for signal that all threads have arrived
        sem_wait(args->sem);
        
        // Signal the next waiting thread
        sem_post(args->sem);
    } else {
        // All threads have arrived
        printf("Thread %d: All threads have arrived! Signaling others.\n", args->i);
        pthread_mutex_unlock(args->mutex);
        
        // Signal one waiting thread, who will signal the next, etc.
        sem_post(args->sem);
    }
    
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
    int count = 0;  // Shared counter for rendez-vous

    // Initialize random seed
    srand(time(NULL));

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_t thread[N];
    sem_t sem;
    sem_init(&sem, 0, 0);  // Initialize semaphore to 0 (no thread can pass initially)

    struct structArg args[N];
    
    for (int i = 0; i < N; i++) {
        args[i].i = i;
        args[i].N = N;
        args[i].sem = &sem;
        args[i].mutex = &mutex;
        args[i].count = &count;
        pthread_create(&thread[i], NULL, task1, &args[i]);  // Fix: pass &args[i] instead of &args
    }
    
    for (int i = 0; i < N; i++)
        pthread_join(thread[i], NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);

    return 0;
}