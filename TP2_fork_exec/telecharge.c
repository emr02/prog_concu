#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PROCESS 4

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <url1> <url2> ... <dossier_destination>\n", argv[0]);
        return 1;
    }

    int dest_idx = argc - 1; // Dernier argument = dossier destination
    int active_processes = 0;

    for (int i = 1; i < dest_idx; i++) {
        // Si on atteint 4 processus, on attend
        if (active_processes >= MAX_PROCESS) {
            wait(NULL);
            active_processes--;
        }

        pid_t pid = fork();
        if (pid == 0) { // Fils
            printf("Début téléchargement %s (PID: %d)\n", argv[i], getpid());
            execlp("wget", "wget", "-P", argv[dest_idx], argv[i], NULL);
            exit(0);
        } else if (pid > 0) { // Parent
            active_processes++;
        }
    }

    // Attend tous les processus restants
    while (active_processes > 0) {
        int status;
        pid_t finished_pid = wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Téléchargement terminé (PID: %d)\n", finished_pid);
        }
        active_processes--;
    }

    return 0;
}