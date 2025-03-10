#include "../dijkstra.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
int main()
{
    int shmid = shmget(5942, sizeof(int), 0);
    shmctl(shmid, IPC_RMID, NULL);
    sem_delete(sem_get(1));
    sem_delete(sem_get(2));
}
