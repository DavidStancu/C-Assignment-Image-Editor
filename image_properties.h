#ifndef IMAGE_PROPERTIES_H_
#define IMAGE_PROPERTIES_H_

typedef struct {
	float r, b, g, grey;
	int selected;
} spectrum;

typedef struct {
	int height, width;
	int iscolor;
	int maxcolor;
	int loadimg;
	int x1, x2, y1, y2;
	char name[300];
	char type[300];
	spectrum **mat;
} image;

void free_memory(image *placeholder);
void allocate_memory_grey(image *placeholder, int height, int width);
void allocate_memory_color(image *placeholder, int height, int width);
void resize_image(image *placeholder);

#endif
