#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

int xy_to_index(int x, int y, size_t width)
{
	return (y * 3 * width) + (3 * x);
}

char* readline(int fd)
{
	int offset = 0;
	size_t size;
	char dummy;
	do {
		size = read(fd,&dummy,1);
		offset += size;
	} while(size != 0 && dummy != '\n' && dummy != EOF);

	char* line = malloc(offset * sizeof(char)+1);
	lseek(fd,-offset,SEEK_CUR);
	read(fd,line,offset);
	line[offset]='\0';
	return line;
}

void load_image(const char* filepath, unsigned char** image_data, size_t* width, size_t* height)
{
	int fd = open(filepath,O_RDONLY);
	if (fd == -1)
	{
		perror("Can't open input file");
		exit(-1);
	}
	char* line = readline(fd);
	if ( strncmp("P6", line, 2) != 0)
	{
		fputs("Input File is not a ppm file\n", stderr);
		free(line);
		exit(-1);
	}
	free(line);
	do {
		line = readline(fd);
		if (line[0] == '#')
		{
			free(line);
			continue;
		}
		int success = sscanf(line,"%lu %lu",width,height);
		if (success != 2)
		{
			fputs("Input File is not a valid ppm file\n", stderr);
			free(line);
			exit(-1);
		}
		free(line);
	} while(*width == 0);
	int dummy = 0;
	do {
		line = readline(fd);
		if (line[0] == '#')
		{
			free(line);
			continue;
		}
		int success = sscanf(line,"%d",&dummy);
		if (success != 1)
		{
			fputs("Input File is not a valid ppm file\n", stderr);
			free(line);
			exit(-1);
		}
		free(line);
	} while(dummy == 0);

	unsigned data_size = 3 * (*width) * (*height);
	*image_data = malloc(data_size * sizeof(unsigned char));

    read(fd,*image_data,3*(*width)*(*height));
}

void save_image(const char* filepath, unsigned char* image, size_t width, size_t height)
{
	int fd = open(filepath,O_CREAT|O_WRONLY,0644);
	if (fd == -1)
	{
		perror("Can't open output file");
		exit(-1);
	}
	write(fd,"P6\n",3);
	char* size = NULL;
	asprintf(&size,"%lu %lu\n",width, height);
	write(fd,size,strlen(size));
	free(size);
	write(fd,"255\n",4);
    usleep(500000);
    write(fd,image,3*width*height);
	write(fd,"\n",1);
	close(fd);

}

void calcul(unsigned char* image,unsigned char* out_image, int width, int height, int x, int y)
{
    int number = 0;
    int somme_r = 0;
    int somme_g = 0;
    int somme_b = 0;
    for (int xi = -1; xi <= 1; xi++)
    {
        for (int yi = -1; yi <= 1; yi++)
        {
            if (x+xi >= 0  && x+xi < width && y+yi >= 0 && y+yi < height)
            {
			    int idx = xy_to_index(x+xi,y+yi,width);
                somme_r += image[idx];
                somme_g += image[idx+1];
                somme_b += image[idx+2];
                number++;
            }
        }
    }
	int idx = xy_to_index(x,y,width);
    out_image[idx] = somme_r/number;
    out_image[idx+1] = somme_g/number;
    out_image[idx+2] = somme_b/number;
}

void flou(const char* filepath, const char* output_dir)
{
	unsigned char* image = NULL;
	unsigned char* out_image = NULL;
    char* filepath_copy = strdup(filepath);
    const char* filename = basename(filepath_copy);
    char* output_filename = malloc((strlen(output_dir)+strlen(filename)+2) * sizeof(char));
    memset(output_filename,0,(strlen(output_dir)+strlen(filename)+2) * sizeof(char));
    strcpy(output_filename,output_dir);
    strcat(output_filename,"/");
    strcat(output_filename,filename);
    puts(output_filename);
	size_t width = 0;
	size_t height = 0;
	
    load_image(filepath, &image, &width, &height);
	out_image = malloc((width*height*3) * sizeof(unsigned char));

    for (size_t y = 0; y < height; y++)
    {
	    for (size_t x = 0; x < width; x++)
        {
            calcul(image, out_image, width, height, x, y);
        }
    }
	save_image(output_filename, out_image, width, height);
	free(image);
    free(out_image);
    free(output_filename);
    free(filepath_copy);
}

/*----- Zone à Modifier -----*/

/*----- Fin de Zone à Modifier -----*/
int main(int argc, char* argv[])
{
    /*----- Zone à Modifier -----*/
    for(int i = 1; i < argc; i++)
    {
        flou(argv[i],"sortie");
    }
    /*----- Fin de Zone à Modifier -----*/
	return 0;
}