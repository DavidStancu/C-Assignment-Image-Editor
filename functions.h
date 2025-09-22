#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void select(image *placeholder, int x1, int x2, int y1, int y2);
void deselect_all(image *placeholder);
void flip_color(image *placeholder, image *flip_placeholder, int num_flip);
void flip_grey(image *placeholder, image *flip_placeholder, int num_flip);
void filter_color(image *placeholder, image *filt_placeholder,
				  float filter[3][3]);
int clamp(float value, int mini, int maxi);
void set_filter(float fltr1[3][3], float fltr2[3][3],
				float fltr3[3][3], float fltr4[3][3]);
void swap_numbers(int x1, int x2);

#endif
