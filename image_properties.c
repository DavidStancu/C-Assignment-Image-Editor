//Copyright David-Ioan Stancu 312CAb 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "image_properties.h"
#include "functions.h"

void free_memory(image *placeholder)
{
	for (int i = 0; i < placeholder->height; i++)
		free(placeholder->mat[i]);
	free(placeholder->mat);
}

void allocate_memory_grey(image *placeholder, int height, int width)
{
	placeholder->mat = (spectrum **)malloc(height * sizeof(spectrum *));
	for (int i = 0; i < height; i++)
		placeholder->mat[i] = (spectrum *)malloc(width * sizeof(spectrum));
}

void allocate_memory_color(image *placeholder, int height, int width)
{
	placeholder->mat = (spectrum **)malloc(height * sizeof(spectrum *));
	for (int i = 0; i < height; i++)
		placeholder->mat[i] = (spectrum *)malloc(width * 3 * sizeof(spectrum));
}

void resize_image(image *placeholder)
{
	free_memory(placeholder);
	FILE *file = fopen(placeholder->name, "r");
	fscanf(file, "%s", placeholder->type);
	fscanf(file, "%d%d", &placeholder->width, &placeholder->height);
	fscanf(file, "%d\n", &placeholder->maxcolor);
	int PPM = (strcmp(placeholder->type, "P3") == 0 ||
			   strcmp(placeholder->type, "P6") == 0);
	if (PPM)
		placeholder->iscolor = 1;
	else
		placeholder->iscolor = 0;
	if (strcmp(placeholder->type, "P2") == 0 ||
		strcmp(placeholder->type, "P3") == 0) {
		if (placeholder->type[1] == '2') {
			allocate_memory_grey(placeholder, placeholder->height,
								 placeholder->width);
			for (int i = 0; i < placeholder->height; i++)
				for (int j = 0; j < placeholder->width; j++)
					fscanf(file, "%f", &placeholder->mat[i][j].grey);
		} else {
			allocate_memory_color(placeholder, placeholder->height,
								  placeholder->width);
			for (int i = 0; i < placeholder->height; i++)
				for (int j = 0; j < placeholder->width; j++) {
					fscanf(file, "%f", &placeholder->mat[i][j].r);
					fscanf(file, "%f", &placeholder->mat[i][j].g);
					fscanf(file, "%f", &placeholder->mat[i][j].b);
				}
		}
		fclose(file);
	}
	if (strcmp(placeholder->type, "P5") == 0 ||
		strcmp(placeholder->type, "P6") == 0) {
		if (placeholder->type[1] == '5') {
			allocate_memory_grey(placeholder, placeholder->height,
								 placeholder->width);
			for (int i = 0; i < placeholder->height; i++)
				for (int j = 0; j < placeholder->width; j++) {
					unsigned char pixel;
					fread(&pixel, 1, 1, file);
					placeholder->mat[i][j].grey = (float)pixel;
				}
		} else {
			allocate_memory_color(placeholder, placeholder->height,
								  placeholder->width);
			for (int i = 0; i < placeholder->height; i++)
				for (int j = 0; j < placeholder->width; j++) {
					unsigned char pixel;
					fread(&pixel, 1, 1, file);
					placeholder->mat[i][j].r = (float)pixel;
					fread(&pixel, 1, 1, file);
					placeholder->mat[i][j].g = (float)pixel;
					fread(&pixel, 1, 1, file);
					placeholder->mat[i][j].b = (float)pixel;
				}
		}
		fclose(file);
	}
	select(placeholder, 0, placeholder->height, 0, placeholder->width);
}
