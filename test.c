#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "dijkstra.h"

int main(){

     int semid = sem_get(1);
     int random = rand() % (5 - 3 + 1) + 3;
     sleep(random);
     printf("fini\n");
     V(semid);
     return 0;
}