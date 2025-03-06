#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
    int tube[2], tube2[2];

    pipe(tube);
    pipe(tube2);

    if (fork() == 0) { // Premier fils : cut
        close(tube[0]); // Ferme la lecture du premier tube
        dup2(open("/etc/passwd", O_RDONLY), 0); // Redirige l'entrée standard vers /etc/passwd
        dup2(tube[1], 1); // Redirige la sortie standard vers tube[1]
        close(tube[1]); // Ferme l'écriture après duplication
        execlp("cut", "cut", "-f", "1,3", "-d", ":", NULL);
        perror("execlp cut");
        exit(1);
    }

    if (fork() == 0) { // Deuxième fils : sed
        close(tube[1]); // Ferme l'écriture du premier tube
        close(tube2[0]); // Ferme la lecture du second tube

        dup2(tube[0], 0); // Lit depuis tube[0]
        dup2(tube2[1], 1); // Écrit vers tube2[1]

        close(tube[0]); // Ferme après duplication
        close(tube2[1]);

        execlp("sed", "sed", "-E", "s/^([^:]+):([^:]+)/\\2:\\1/", NULL);
        perror("execlp sed");
        exit(1);
    }

    // Processus parent : sort
    close(tube[0]);
    close(tube[1]);
    close(tube2[1]); // Ferme l'écriture du deuxième tube

    int output = open("./users", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(tube2[0], 0); // Lit depuis tube2[0]
    dup2(output, 1); // Écrit vers le fichier
    close(tube2[0]);
    close(output);

    execlp("sort", "sort", "-n", NULL);
    perror("execlp sort");
    exit(1);

    return 0;
}
