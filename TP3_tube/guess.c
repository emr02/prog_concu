#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    int tube[2];  // Pipe père -> fils
    int tube2[2]; // Pipe fils -> père
    int n;        // Nombre saisi
    char rep;     // Réponse (+, -, =)

    // Création des pipes avec vérification
    if (pipe(tube) == -1 || pipe(tube2) == -1) {
        perror("Création Pipe");
        exit(-1);
    }

    int pid = fork();
    if (pid == -1) {
        perror("Erreur fork");
        exit(-1);
    }

    if (pid == 0) { // Processus fils
        close(tube[1]);  // Ferme écriture tube
        close(tube2[0]); // Ferme lecture tube2

        srand(time(NULL));      // Initialise rand
        int random = rand() % 50; // Nombre aléatoire 0-49

        while (1) {
            read(tube[0], &n, sizeof(int)); // Lit le nombre du père
            if (n == random) {
                rep = '=';
            } else if (n > random) {
                rep = '-';
            } else {
                rep = '+';
            }
            write(tube2[1], &rep, sizeof(char)); // Envoie réponse au père
            if (rep == '=') break; // Sort si trouvé
        }

        close(tube[0]);
        close(tube2[1]);
        exit(0); // Termine le fils proprement
    } else { // Processus père
        close(tube[0]);  // Ferme lecture tube
        close(tube2[1]); // Ferme écriture tube2

        while (1) {
            printf("Entre un nombre (0-49) : ");
            scanf("%d", &n);
            write(tube[1], &n, sizeof(int)); // Envoie au fils
            read(tube2[0], &rep, sizeof(char)); // Lit réponse

            // Affiche message en fonction de la réponse
            if (rep == '+') {
                printf("Le nombre est plus grand\n");
            } else if (rep == '-') {
                printf("Le nombre est plus petit\n");
            } else if (rep == '=') {
                printf("Bravo, tu as trouvé !\n");
                break; // Sort si trouvé
            }
        }

        close(tube[1]);
        close(tube2[0]);
    }
    return 0;
}