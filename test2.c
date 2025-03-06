#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "dijkstra.h"

int main(){

     int semid = sem_get(1);
     P(semid);
     int random = rand() % 4 + 1;
     sleep(random);
     printf("fini 2\n");
     return 0;
}