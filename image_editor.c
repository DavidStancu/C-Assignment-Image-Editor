//Copyright David-Ioan Stancu 312CAb 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "image_properties.h"
#include "functions.h"
#define MAXBIN 256

void load_file(image *png, char filename[300])//comanda de LOAD
{
	png->loadimg = 0;//anuleaza imaginea originala
	FILE *file = fopen(filename, "r");
	if (!file) {//caut sa vad daca fisierul este citibil
		printf("Failed to load %s\n", filename);
		return;
	}
	fscanf(file, "%s", png->type);
	fscanf(file, "%d%d", &png->width, &png->height);
	fscanf(file, "%d\n", &png->maxcolor);// extrag informatiile din fisier
	int PPM = (strcmp(png->type, "P3") == 0 || strcmp(png->type, "P6") == 0);
	if (PPM)
		png->iscolor = 1;
	else
		png->iscolor = 0;//vad daca e tip color
	if (strcmp(png->type, "P2") == 0 || strcmp(png->type, "P3") == 0) {
		if (!file) {
			printf("Failed to load %s\n", filename);
			return;
		}
		strcpy(png->name, filename);
		if (png->type[1] == '2') {//incarc imagine grayscale
			png->loadimg = 1;
			allocate_memory_grey(png, png->height, png->width);
			for (int i = 0; i < png->height; i++)
				for (int j = 0; j < png->width; j++)
					fscanf(file, "%f", &png->mat[i][j].grey);
		} else {//incarc imagine color
			png->loadimg = 1;
			allocate_memory_color(png, png->height, png->width);
			for (int i = 0; i < png->height; i++)
				for (int j = 0; j < png->width; j++) {
					fscanf(file, "%f", &png->mat[i][j].r);
					fscanf(file, "%f", &png->mat[i][j].g);
					fscanf(file, "%f", &png->mat[i][j].b);
				}
		}
		fclose(file);
	}//incarcare fisier tip ASCII
	if (strcmp(png->type, "P5") == 0 || strcmp(png->type, "P6") == 0) {
		if (!file) {
			fprintf(stderr, "Failed to load %s\n", filename);
			return;
		}
		strcpy(png->name, filename);
		if (png->type[1] == '5') {
			png->loadimg = 1;
			allocate_memory_grey(png, png->height, png->width);
			for (int i = 0; i < png->height; i++)
				for (int j = 0; j < png->width; j++) {
					unsigned char pixel;
					fread(&pixel, 1, 1, file);
					png->mat[i][j].grey = (float)pixel;
				}
		} else {
			png->loadimg = 1;
			allocate_memory_color(png, png->height, png->width);
			for (int i = 0; i < png->height; i++)
				for (int j = 0; j < png->width; j++) {
					unsigned char pixel;
					fread(&pixel, 1, 1, file);
					png->mat[i][j].r = (float)pixel;
					fread(&pixel, 1, 1, file);
					png->mat[i][j].g = (float)pixel;
					fread(&pixel, 1, 1, file);
					png->mat[i][j].b = (float)pixel;
				}
		}//incarcare fisier tip binar
		fclose(file);
	}
	if (png->loadimg)
		printf("Loaded %s\n", filename);
	select(png, 0, png->height, 0, png->width);//selectare ALL default
}

void save_file(image *png, char prompt[500])
{
	if (!png->loadimg) {//vad daca exista un fisier incarcat
		printf("No image loaded\n");
		return;
	}
	char filename[450];
	int c = sscanf(prompt, "%s", filename);
	int isascii = 0;
	if (strstr(prompt, "ascii"))//comenzi care verifica existenta cuvantului ascii
		isascii = 1;
	FILE *file = fopen(filename, "w");
	if (isascii) {
		if (png->type[1] == '5' || png->type[1] == '6')
			png->type[1] -= 3;//schimbarea fortata a tipului
		fprintf(file, "%s\n", png->type);
		fprintf(file, "%d %d\n", png->width, png->height);
		fprintf(file, "%d\n", png->maxcolor);//incarcarea informatiilor despre imagine
		if (png->type[1] == '2') {
			for (int i = 0; i < png->height; i++) {
				for (int j = 0; j < png->width; j++)
					fprintf(file, "%.0f ", png->mat[i][j].grey);
				fprintf(file, "\n");
			}//incarcare a unei imagini grayscale
			printf("Saved %s\n", filename);
			strcpy(png->name, filename);
			fclose(file);
		}
		if (png->type[1] == '3') {
			for (int i = 0; i < png->height; i++) {
				for (int j = 0; j < png->width; j++)
					fprintf(file, "%.0f %.0f %.0f ", round(png->mat[i][j].r),
							round(png->mat[i][j].g), round(png->mat[i][j].b));
				fprintf(file, "\n");
			}//incarcare a unei imagini color
			printf("Saved %s\n", filename);
			strcpy(png->name, filename);
			fclose(file);
		}//incarcarea imaginii tip ASCII
		resize_image(png);
	} else {
		if (png->type[1] == '2' || png->type[1] == '3')
			png->type[1] += 3;
		fprintf(file, "%s\n", png->type);
		fprintf(file, "%d %d\n", png->width, png->height);
		fprintf(file, "%d\n", png->maxcolor);
		if (png->type[1] == '5') {
			for (int i = 0; i < png->height; i++) {
				for (int j = 0; j < png->width; j++) {
					unsigned char pixel;
					pixel = (unsigned char)round(png->mat[i][j].grey);
					fwrite(&pixel, sizeof(unsigned char), 1, file);
				}
			}
			printf("Saved %s\n", filename);
			strcpy(png->name, filename);
			fclose(file);
		}
		if (png->type[1] == '6') {
			for (int i = 0; i < png->height; i++) {
				for (int j = 0; j < png->width; j++) {
					unsigned char pixelr, pixelg, pixelb;
					pixelr = (unsigned char)(round(png->mat[i][j].r));
					pixelg = (unsigned char)(round(png->mat[i][j].g));
					pixelb = (unsigned char)(round(png->mat[i][j].b));
					fwrite(&pixelr, sizeof(unsigned char), 1, file);
					fwrite(&pixelg, sizeof(unsigned char), 1, file);
					fwrite(&pixelb, sizeof(unsigned char), 1, file);
				}
			}
			printf("Saved %s\n", filename);
			strcpy(png->name, filename);
			fclose(file);
		}
		resize_image(png);
	}//incarcarea imaginii tip binar
}

void select_paramaters(image *png, char prompt[500])//comanda SELECT
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	deselect_all(png);
	if (strstr(prompt, "ALL") != 0) {
		select(png, 0, png->height, 0, png->width);
		printf("Selected ALL\n");
		return;
	}
	int x1, y1, x2, y2;
	int charcheck = 0, nrcount = 0;
	prompt[strlen(prompt) - 1] = '\0';
	char *ptr = strtok(prompt, " ");
	while (ptr && nrcount < 4) {
		char *endl;
		int nr = strtol(ptr, &endl, 10);
		if (*endl == '\0') {
			if (nrcount == 0)
				x1 = nr;
			else if (nrcount == 1)
				y1 = nr;
			else if (nrcount == 2)
				x2 = nr;
			else if (nrcount == 3)
				y2 = nr;
			nrcount++;
		} else {
			charcheck = 1;
		}
		ptr = strtok(NULL, " ");
	}//extragerea variabilelor din string
	if (nrcount < 4 || charcheck) {
		printf("Invalid command\n");
		return;
	}
	if ((x1 < 0 || y1 < 0 || x2 > png->width || y2 > png->height) ||
		x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 > x2) {
		int aux = x1;
		x1 = x2;
		x2 = aux;
	}
	if (y1 > y2) {
		int aux = y1;
		y1 = y2;
		y2 = aux;
	}
	select(png, y1, y2, x1, x2);//selectarea coordonatelor
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

void show_histogram(image *png, char prompt[300])// comanda HISTOGRAM
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	int x, y;
	prompt[strlen(prompt) - 1] = '\0';
	char *ptr = strtok(prompt, " ");
	int nrcount = 0;
	while (ptr) {
		char *endl;
		int nr = strtol(ptr, &endl, 10);
		if (*endl == '\0') {
			if (nrcount == 0)
				x = nr;
			else if (nrcount == 1)
				y = nr;
			nrcount++;
		}
		ptr = strtok(NULL, " ");
	}
	if (nrcount != 2 || prompt[0] == '\0') {
		printf("Invalid command\n");
		return;
	}
	if (png->iscolor) {
		printf("Black and white image needed\n");
		return;
	}
	if (x <= 0 || y % 2 != 0) {
		printf("Invalid set of parameters\n");
		return;
	}
		int maxfrecv = 0;
		int frecv[MAXBIN] = {0};
		for (int i = 0; i < png->height; i++)
			for (int j = 0; j < png->width; j++)
				frecv[(int)(png->mat[i][j].grey)]++;
		int binfrecv[MAXBIN] = {0};
		int aux = 256 / y;
		int j = 0;
		for (int i = 0; i < y; i++)
			for (int k = 0; k < aux && j < 256; k++) {
				binfrecv[i] += frecv[j];
				j++;
			}
		for (int i = 0; i < 256; i++)
			if (maxfrecv < binfrecv[i])
				maxfrecv = binfrecv[i];
		for (int i = 0; i < y; i++) {
			int star = round((float)((binfrecv[i] * x) / maxfrecv));
			printf("%d\t|\t", star);
			for (int j = 0; j < star; j++)
				printf("*");
			printf("\n");
		}
}

void rotate_file(image *png, int angle)// comanda ROTATE
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	if (((png->y2 - png->y1) != (png->x2 - png->x1)) && (png->x1 != 0 ||
		  png->x2 != png->height || png->y1 != 0 || png->y2 != png->width)) {
		printf("The selection must be square\n");
		return;
	}
	if ((abs(angle) % 90 == 0 && (abs(angle) <= 360) && abs(angle) >= 0)) {
		image *flipmat;
		flipmat = malloc(sizeof(image));
		flipmat->height = png->x2 - png->x1;
		flipmat->width = png->y2 - png->y1;
		int transp_nr = angle / 90;
		if (png->x1 == 0 && png->y1 == 0 && png->x2 == png->height &&
			png->y2 == png->width) {
			if (png->iscolor) {
				if (transp_nr < 0)
					transp_nr = 4 + transp_nr;
				for (int k = 0; k < transp_nr; k++) {
					allocate_memory_color(flipmat, png->width, png->height);
				for (int i = 0; i < png->width; i++)
					for (int j = 0; j < png->height; j++) {
						int j2 = png->height - j - 1;
						flipmat->mat[i][j2].r = png->mat[j][i].r;
						flipmat->mat[i][j2].g = png->mat[j][i].g;
						flipmat->mat[i][j2].b = png->mat[j][i].b;
					}
				free_memory(png);
				int aux = png->height;
				png->height = png->width;
				png->width = aux;
				png->mat = flipmat->mat;
				png->x2 = png->height;
				png->y2 = png->width;
				}
			} else {//rotire pe toata imaginea
				if (transp_nr < 0)
					transp_nr = 4 + transp_nr;
				for (int k = 0; k < transp_nr; k++) {
					allocate_memory_grey(flipmat, png->width, png->height);
				for (int i = 0; i < png->width; i++)
					for (int j = 0; j < png->height; j++) {
						int j2 = png->height - j - 1;
						flipmat->mat[i][j2].grey = png->mat[j][i].grey;
					}
				free_memory(png);
				int aux = png->height;
				png->height = png->width;
				png->width = aux;
				png->mat = flipmat->mat;
				png->x2 = png->height;
				png->y2 = png->width;
				}
			}
		} else {
			if (transp_nr < 0)
				transp_nr = 4 + transp_nr;
			if (png->iscolor) {
				allocate_memory_color(flipmat, png->height, png->width);
				flip_color(png, flipmat, transp_nr);
			} else {
				allocate_memory_grey(flipmat, png->width, png->height);
				flip_grey(png, flipmat, transp_nr);
			}
		}//rotire pe o bucata
		free(flipmat);
		printf("Rotated %d\n", angle);
	} else {
		printf("Unsupported rotation angle\n");
		return;
	}
}

void crop_file(image *png)//comanda CROP
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	image *crop_png;
	crop_png = malloc(sizeof(image));
	crop_png->height = png->x2 - png->x1;
	crop_png->width = png->y2 - png->y1;
	if (png->iscolor) {
		allocate_memory_color(crop_png, crop_png->height, crop_png->width);
		for (int i = 0; i < crop_png->height; i++) {
			for (int j = 0; j < crop_png->width; j++) {
				int i2 = png->x1 + i;
				int j2 = png->y1 + j;
				crop_png->mat[i][j].r = png->mat[i2][j2].r;
				crop_png->mat[i][j].g = png->mat[i2][j2].g;
				crop_png->mat[i][j].b = png->mat[i2][j2].b;
			}
		}
		free_memory(png);
		png->width = crop_png->width;
		png->height = crop_png->height;
		allocate_memory_color(png, png->height, png->width);
		for (int i = 0; i < crop_png->height; i++) {
			for (int j = 0; j < crop_png->width; j++) {
				png->mat[i][j].r = crop_png->mat[i][j].r;
				png->mat[i][j].g = crop_png->mat[i][j].g;
				png->mat[i][j].b = crop_png->mat[i][j].b;
			}
		}
		select(png, 0, png->height, 0, png->width);
		free_memory(crop_png);
		free(crop_png);
		printf("Image cropped\n");
	} else {
		allocate_memory_color(crop_png, crop_png->height,
							  crop_png->width);
		for (int i = 0; i < crop_png->height; i++)
			for (int j = 0; j < crop_png->width; j++) {
				int i2 = png->x1 + i;
				int j2 = png->y1 + j;
				crop_png->mat[i][j].grey = png->mat[i2][j2].grey;
			}
		free_memory(png);
		png->width = crop_png->width;
		png->height = crop_png->height;
		allocate_memory_grey(png, png->height, png->width);
		for (int i = 0; i < crop_png->height; i++) {
			for (int j = 0; j < crop_png->width; j++)
				png->mat[i][j].grey = crop_png->mat[i][j].grey;
		}
		select(png, 0, png->height, 0, png->width);
		free_memory(crop_png);
		free(crop_png);
		printf("Image cropped\n");
	}
}

void equalize_file(image *png)//comanda EQUALIZE
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	if (png->iscolor) {
		printf("Black and white image needed\n");
		return;
	}
	int pixelfrecv[MAXBIN] = {0};
	float png_area = png->height * png->width;
	for (int i = 0; i < png->height; i++)
		for (int j = 0; j < png->width; j++)
			pixelfrecv[(int)round(png->mat[i][j].grey)]++;
	for (int i = 0; i < png->height; i++) {
		for (int j = 0; j < png->width; j++) {
			float sum_pix_prec = 0;
			float changed_pix_val;
			for (int cnt = 0; cnt <= round(png->mat[i][j].grey); cnt++)
				sum_pix_prec += pixelfrecv[cnt];
			changed_pix_val = (float)round((255 * sum_pix_prec) / png_area);
			float newpixel = round(clamp(changed_pix_val, 0, 255));
			png->mat[i][j].grey = newpixel;
		}
	}
	printf("Equalize done\n");
}

void apply_filter(image *png, char filter[500])//comanda APPLY
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	filter[strlen(filter) - 1] = '\0';
	image *filter_png;
	float edge[3][3] = {0}, sharpen[3][3] = {0}, box_blur[3][3] = {0};
	float gauss_blur[3][3] = {0};
	set_filter(edge, sharpen, box_blur, gauss_blur);
	if (png->iscolor) {
		filter_png = malloc(sizeof(image));
		filter_png->height = png->x2 - png->x1;
		filter_png->width = png->y2 - png->y1;
		allocate_memory_color(filter_png, png->height, png->width);
		if (strstr(filter, "EDGE") != 0) {
			filter_color(png, filter_png, edge);
			for (int i = png->x1; i < png->x2; i++)
				for (int j = png->y1; j < png->y2; j++) {
					png->mat[i][j].r = clamp(png->mat[i][j].r, 0, 255);
					png->mat[i][j].g = clamp(png->mat[i][j].g, 0, 255);
					png->mat[i][j].b = clamp(png->mat[i][j].b, 0, 255);
				}
			printf("APPLY%s done\n", filter);
		} else if (strstr(filter, "SHARPEN") != 0) {
			filter_color(png, filter_png, sharpen);
			for (int i = png->x1; i < png->x2; i++)
				for (int j = png->y1; j < png->y2; j++) {
					png->mat[i][j].r = clamp(round(png->mat[i][j].r), 0, 255);
					png->mat[i][j].g = clamp(round(png->mat[i][j].g), 0, 255);
					png->mat[i][j].b = clamp(round(png->mat[i][j].b), 0, 255);
				}
			printf("APPLY%s done\n", filter);
		} else if (strstr(filter, " BLUR") != 0) {
			filter_color(png, filter_png, box_blur);
			for (int i = png->x1; i < png->x2; i++)
				for (int j = png->y1; j < png->y2; j++) {
					png->mat[i][j].r = clamp(round(png->mat[i][j].r), 0, 255);
					png->mat[i][j].g = clamp(round(png->mat[i][j].g), 0, 255);
					png->mat[i][j].b = clamp(round(png->mat[i][j].b), 0, 255);
				}
			printf("APPLY%s done\n", filter);
		} else if (strstr(filter, "GAUSSIAN_BLUR") != 0) {
			filter_color(png, filter_png, gauss_blur);
			for (int i = png->x1; i < png->x2; i++)
				for (int j = png->y1; j < png->y2; j++) {
					png->mat[i][j].r = clamp(round(png->mat[i][j].r), 0, 255);
					png->mat[i][j].g = clamp(round(png->mat[i][j].g), 0, 255);
					png->mat[i][j].b = clamp(round(png->mat[i][j].b), 0, 255);
				}
			printf("APPLY%s done\n", filter);
		} else {
			printf("APPLY parameter invalid\n");
		}
		free_memory(filter_png);
	} else if (filter[0] != '\0' && (png->type[1] == '2' ||
			   png->type[1] == '5')) {
		printf("Easy, Charlie Chaplin\n");
		return;
	} else if (filter[0] == '\0') {
		printf("Invalid command\n");
		return;
	}
}

void program_end(image *png)
{
	if (!png->loadimg) {
		printf("No image loaded\n");
		return;
	}
	free_memory(png);
	png->loadimg = 0;
	free(png);
}

int main(void)
{
	image *png;
	png = malloc(sizeof(image));
	png->loadimg = 0;
	char c[300];
	char filename[300];
	do {
		scanf("%s", c);
		if (strcmp(c, "LOAD") == 0) {
			scanf("%s", filename);
			load_file(png, filename);
		} else if (strcmp(c, "SELECT") == 0) {
			char prompt[500];
			fgets(prompt, sizeof(prompt), stdin);
			select_paramaters(png, prompt);
		} else if (strcmp(c, "HISTOGRAM") == 0) {
			char prompt[500];
			fgets(prompt, sizeof(prompt), stdin);
			show_histogram(png, prompt);
		} else if (strcmp(c, "EQUALIZE") == 0) {
			equalize_file(png);
		} else if (strcmp(c, "ROTATE") == 0) {
			int angle;
			scanf("%d", &angle);
			rotate_file(png, angle);
		} else if (strcmp(c, "CROP") == 0) {
			crop_file(png);
		} else if (strcmp(c, "APPLY") == 0) {
			char filter[500];
			fgets(filter, sizeof(filter), stdin);
			apply_filter(png, filter);
		} else if (strcmp(c, "SAVE") == 0) {
			char prompt[500];
			fgets(prompt, sizeof(prompt), stdin);
			save_file(png, prompt);
		} else if (strcmp(c, "EXIT") == 0) {
			program_end(png);
		} else {
			printf("Invalid command\n");
		}
	} while (strcmp(c, "EXIT") != 0);
	return 0;
}
