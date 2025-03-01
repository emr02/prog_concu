#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

int xy_to_index(int x, int y, size_t width)
{
    return (y * 3 * width) + (3 * x);
}

char *readline(int fd)
{
    int offset = 0;
    size_t size;
    char dummy;
    do
    {
        size = read(fd, &dummy, 1);
        offset += size;
    } while (size != 0 && dummy != '\n' && dummy != EOF);

    char *line = malloc(offset * sizeof(char) + 1);
    lseek(fd, -offset, SEEK_CUR);
    read(fd, line, offset);
    line[offset] = '\0';
    return line;
}

void load_image(char *filepath, unsigned char **image_data, size_t *width, size_t *height)
{
    int fd = open(filepath, O_RDONLY);
    if (fd == -1)
    {
        perror("Can't open input file");
        exit(-1);
    }
    char *line = readline(fd);
    if (strncmp("P6", line, 2) != 0)
    {
        fputs("Input File is not a ppm file\n", stderr);
        free(line);
        exit(-1);
    }
    free(line);
    do
    {
        line = readline(fd);
        if (line[0] == '#')
        {
            free(line);
            continue;
        }
        int success = sscanf(line, "%lu %lu", width, height);
        if (success != 2)
        {
            fputs("Input File is not a valid ppm file\n", stderr);
            free(line);
            exit(-1);
        }
        free(line);
    } while (*width == 0);
    int dummy = 0;
    do
    {
        line = readline(fd);
        if (line[0] == '#')
        {
            free(line);
            continue;
        }
        int success = sscanf(line, "%d", &dummy);
        if (success != 1)
        {
            fputs("Input File is not a valid ppm file\n", stderr);
            free(line);
            exit(-1);
        }
        free(line);
    } while (dummy == 0);

    unsigned data_size = 3 * (*width) * (*height);
    *image_data = malloc(data_size * sizeof(unsigned char));

    read(fd, *image_data, 3 * (*width) * (*height));
}

void save_image(char *filepath, unsigned char *image, size_t width, size_t height)
{
    int fd = open(filepath, O_CREAT | O_WRONLY, 0644);
    if (fd == -1)
    {
        perror("Can't open output file");
        exit(-1);
    }
    write(fd, "P6\n", 3);
    char *size = NULL;
    asprintf(&size, "%lu %lu\n", width, height);
    write(fd, size, strlen(size));
    free(size);
    write(fd, "255\n", 4);
    write(fd, image, 3 * width * height);
    write(fd, "\n", 1);
    close(fd);
}

void calcul(unsigned char *image, unsigned char *out_image, int width, int height, int x, int y)
{
    assert(x >= 0);
    assert(x < width);
    assert(y >= 0);
    assert(y < height);

    int idx = xy_to_index(x, y, width);
    int red = image[idx];
    int green = image[idx + 1];
    int blue = image[idx + 2];
    int new_red = 0;
    int new_green = 0;
    int new_blue = 0;
    for (int i = 0; i <= 1000; i++)
    {
        float fact = 393.0f / i;
        new_red = fact * red + 0.769 * green + 0.189 * blue;
        fact = 686.0f / i;
        new_green = 0.349 * red + fact * green + 0.168 * blue;
        fact = 131.0f / i;
        new_blue = 0.272 * red + 0.534 * green + fact * blue;
    }
    out_image[idx] = new_red <= 255 ? new_red : 255;
    out_image[idx + 1] = new_green <= 255 ? new_green : 255;
    out_image[idx + 2] = new_blue <= 255 ? new_blue : 255;
}

/*----- Zone à Modifier -----*/

// structure thread_data pour passer les données nécessaires à chaque thread
struct thread_data
{
    unsigned char *image;
    unsigned char *out_image;
    size_t width;
    size_t height;
    size_t *current_row;
    pthread_mutex_t *mutex;
};

// chaque thread exécutera cette fonction
void *thread_function(void *arg)
{
    //(struct thread_data *)arg peut être que arg, c'est plus simple, c'est pas recommandé
    // On récupère les données passées au thread
    struct thread_data *data = (struct thread_data *)arg;

    // Boucle infinie pour traiter les lignes de l'image
    while (1)
    {
        // On verrouille le mutex pour accéder à current_row de manière sécurisée.
        pthread_mutex_lock(data->mutex);
        // On lit la ligne actuelle à traiter.
        size_t row = *(data->current_row);
        //Si toutes les lignes ont été traitées, on sort de la boucle
        if (row >= data->height)
        {
            //On déverrouille le mutex avant de sortir.
            pthread_mutex_unlock(data->mutex);
            break;
        }
        //On incrémente current_row pour indiquer que cette ligne est en cours de traitement.
        *(data->current_row) += 1;
        //On déverrouille le mutex.
        pthread_mutex_unlock(data->mutex);

        //On traite chaque pixel de la ligne.
        for (unsigned int x = 0; x < data->width; x++)
        {
            //On applique l'effet sépia au pixel (x, row).
            calcul(data->image, data->out_image, data->width, data->height, x, row);
        }
    }

    return NULL;
}

/*----- Fin de Zone à Modifier -----*/

int main(void)
{
    unsigned char *image = NULL;
    unsigned char *out_image = NULL;
    size_t width = 0;
    size_t height = 0;

    // chargement de l'image
    load_image("image.ppm", &image, &width, &height);
    // creation d'une image vide de taille identique à l'image d'origine
    out_image = malloc((width * height * 3) * sizeof(unsigned char));

    /*----- Zone à Modifier -----*/
    
    // calcul de chacun des pixels d'origine pour remplir les pixels de l'image de sortie
    /*for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            calcul(image, out_image, width, height, x, y);
        }
    }*/

    size_t current_row = 0; //Variable partagée pour indiquer la ligne actuelle à traiter.
    pthread_mutex_t mutex; //Mutex pour synchroniser l'accès à current_row.
    pthread_mutex_init(&mutex, NULL); //Initialisation du mutex.

    //Initialisation de la structure thread_data.
    struct thread_data data = {image, out_image, width, height, &current_row, &mutex}; 
    pthread_t threads[4]; //Tableau pour stocker les identifiants des threads.

    //Création des 4 threads.
    for (int i = 0; i < 4; i++)
    {
        //Création d'un thread qui exécute thread_function.
        pthread_create(&threads[i], NULL, thread_function, &data); 
    }

    for (int i = 0; i < 4; i++) //Attente de la fin des 4 threads
    {
        pthread_join(threads[i], NULL); //Attente de la fin du thread.
    }

    pthread_mutex_destroy(&mutex); //Destruction du mutex.

    /*----- Fin de Zone à Modifier -----*/
    // sauvegarde de l'image
    save_image("sepia.ppm", out_image, width, height);
    // libération de la mémoire
    free(image);
    free(out_image);
    return 0;
}


/*

Premier Thread:

Le premier thread verrouille le mutex et lit current_row (qui est 0).
Il incrémente current_row à 1.
Il déverrouille le mutex.
Il traite la ligne 0.
Deuxième Thread:

Le deuxième thread verrouille le mutex et lit current_row (qui est maintenant 1).
Il incrémente current_row à 2.
Il déverrouille le mutex.
Il traite la ligne 1.


Verrouillage du Mutex:

Le thread verrouille le mutex pour accéder à current_row de manière sécurisée.
Lecture de current_row:

Le thread lit la valeur actuelle de current_row, qui indique la ligne à traiter.
Incrémentation de current_row:

Le thread incrémente current_row pour indiquer que cette ligne est en cours de traitement et que la prochaine ligne doit être traitée par le prochain thread.
Déverrouillage du Mutex:

Le thread déverrouille le mutex pour permettre à d'autres threads d'accéder à current_row.
Traitement de la Ligne:

Le thread traite chaque pixel de la ligne row en appelant la fonction calcul.

*/