#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    int tube[2];
    int res = pipe(tube);
    if(res == -1){
        perror("Création Pipe");
        exit(-1);
    }
    if(fork()==0){
        close(tube[0]);
        //ouvre le fichier server.log en lecture
        int input = open("./server.log",O_RDONLY);
        //redirige l'entrée du fichier dans l'entrée standard
        dup2(input, 0);
        dup2(tube[1], 1);
        close(tube[1]);
        execlp("grep","grep", "invalid credentials", NULL);
    }
    else{
        int tube2[2];
        int res2 = pipe(tube2);
        if(res2 == -1){
            perror("Création Pipe2");
            exit(-1);
        }
        close(tube[1]);
        if(fork()==0){
            close(tube2[0]);
            dup2(tube[0], 0);
            dup2(tube2[1], 1);
            close(tube[0]);
            close(tube2[1]);
            execlp("cut", "cut", "-d,", "-f3", NULL);
        }else{
            close(tube[0]);
            close(tube2[1]);
            //ouvre le fichier de sortie en écriture, le crée si il n'existe pas et le vide
            int input = open("./sortie",O_WRONLY | O_CREAT | O_TRUNC, 0644);
            //redirige la sortie standard dans le fichier
            dup2(input, 1);
            //redirige l'entrée standard dans le tube
            dup2(tube2[0], 0);
            close(tube2[0]);
            execlp("sort","sort", NULL);
        }
    }
}

/*

Je comprends votre préoccupation. En fait, les appels à sem_create dans prod.c et conso.c
 ne créent pas de nouveaux sémaphores, mais accèdent aux sémaphores existants créés dans launcher.c. 
La valeur passée à sem_create dans prod.c et conso.c n'a pas d'importance tant que le sémaphore existe déjà.

Pourquoi cela fonctionne
En remplaçant `SEM` par `MEM` dans la création du sémaphore `sem_prod`, vous avez modifié le nombre initial 
de ressources disponibles pour le sémaphore `sem_prod`. Voici pourquoi cela a fonctionné :


### Explication
- **Initialisation avec `SEM` (5)** : Le sémaphore `sem_prod` est initialisé à 5, 
ce qui signifie que le producteur peut produire jusqu'à 5 éléments avant que le consommateur ne commence à consommer.
- **Initialisation avec `MEM` (3)** : Le sémaphore `sem_prod` est initialisé à 3, 
ce qui signifie que le producteur peut produire jusqu'à 3 éléments avant que le consommateur ne commence à consommer.

### Pourquoi cela a fonctionné
En initialisant `sem_prod` avec `MEM` (3), vous avez limité le nombre de productions initiales à 3, 
ce qui correspond à la taille de la mémoire partagée. Cela empêche le producteur de produire plus de données 
que la mémoire partagée ne peut en contenir, évitant ainsi les écrasements de données et les lectures incorrectes.

### Conclusion
En utilisant `MEM` pour initialiser `sem_prod`, vous avez aligné la capacité de production initiale 
avec la taille de la mémoire partagée, ce qui a résolu le problème d'affichage incorrect.

*/