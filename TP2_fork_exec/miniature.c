#include <stdio.h>      // Pour les entrées/sorties (printf, perror, etc.)
#include <dirent.h>     // Pour manipuler les répertoires (opendir, readdir, etc.)
#include <unistd.h>     // Pour les appels système (fork, execlp, chdir, etc.)

int main(int argc, char *argv[]) {  // argc = nb d'arguments, argv = tableau des arguments
    DIR *dir = opendir(argv[1]);    // Ouvre le dossier donné en argument (ex. /home/wind/.../images)
                                    // argv[1] est le chemin fourni lors de l'exécution

    struct dirent *entry;           // Déclare une structure pour stocker les infos de chaque fichier

    chdir(argv[1]);                 // Change le répertoire courant vers le dossier spécifié
                                    // Ainsi, convert travaillera directement dans ce dossier

    while ((entry = readdir(dir)) != NULL) { // Boucle tant qu'il y a des éléments dans le dossier
                                             // readdir renvoie un pointeur vers l'entrée suivante
        if (entry->d_type == DT_REG) {       // Vérifie si l'entrée est un fichier régulier (pas un dossier)
                                             // DT_REG = fichier standard, exclut . et .. automatiquement

            if (fork() == 0) {               // Crée un processus fils avec fork()
                                             // fork() renvoie 0 dans le fils, PID du fils dans le parent
                // Code exécuté par le fils
                execlp("convert", "convert", // Appelle l'utilitaire convert d'ImageMagick
                       entry->d_name,        // Nom du fichier source (ex. 21.jpg)
                       "-resize", "10%",     // Option pour redimensionner à 10% de la taille originale
                       entry->d_name,        // Nom du fichier de sortie (écrase l'original ici)
                       NULL);                // Termine la liste d'arguments pour execlp
                exit(0);                     // Quitte le fils après conversion (ou si execlp échoue)
            }
            // Le parent continue la boucle sans attendre le fils
        }
    }

    closedir(dir);    // Ferme le dossier une fois qu'on a fini de le parcourir
    return 0;         // Fin du programme, renvoie 0 pour indiquer succès
}