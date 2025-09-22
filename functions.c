//Copyright David-Ioan Stancu 312CAb 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "image_properties.h"
#include "functions.h"

void select(image *placeholder, int x1, int x2, int y1, int y2)
{
	deselect_all(placeholder);
	for (int i = x1; i < x2; i++)
		for (int j = y1; j < y2; j++)
			placeholder->mat[i][j].selected = 1;
	placeholder->x1 = x1;
	placeholder->x2 = x2;
	placeholder->y1 = y1;
	placeholder->y2 = y2;
}

void deselect_all(image *placeholder)
{
	for (int i = 0; i < placeholder->height; i++)
		for (int j = 0; j < placeholder->width; j++)
			placeholder->mat[i][j].selected = 0;
}

void flip_color(image *placeholder, image *flip_pimg, int num_flip)
{
	for (int k = 0; k < num_flip; k++) {
		for (int i = placeholder->x1; i < placeholder->x2; i++)
			for (int j = placeholder->y1; j < placeholder->y2; j++) {
				flip_pimg->mat[i][j].r = placeholder->mat[i][j].r;
				flip_pimg->mat[i][j].g = placeholder->mat[i][j].g;
				flip_pimg->mat[i][j].b = placeholder->mat[i][j].b;
			}
		for (int i = placeholder->x1; i < placeholder->x2; i++)
			for (int j = placeholder->y1; j < placeholder->y2; j++) {
				int flipi = j - placeholder->y1 + placeholder->x1;
				int flipj = placeholder->y2 + placeholder->x1 - i - 1;
				placeholder->mat[flipi][flipj].r = flip_pimg->mat[i][j].r;
				placeholder->mat[flipi][flipj].g = flip_pimg->mat[i][j].g;
				placeholder->mat[flipi][flipj].b = flip_pimg->mat[i][j].b;
			}
		for (int i = placeholder->x1; i < placeholder->x2; i++)
			for (int j = placeholder->y1; j < placeholder->y2; j++) {
				flip_pimg->mat[i][j].r = placeholder->mat[i][j].r;
				flip_pimg->mat[i][j].g = placeholder->mat[i][j].g;
				flip_pimg->mat[i][j].b = placeholder->mat[i][j].b;
			}
	}
}

void flip_grey(image *placeholder, image *flip_pimg, int num_flip)
{
	for (int k = 0; k < num_flip; k++) {
		for (int i = placeholder->x1; i < placeholder->x2; i++)
			for (int j = placeholder->y1; j < placeholder->y2; j++)
				flip_pimg->mat[i][j].grey = placeholder->mat[i][j].grey;
		for (int i = placeholder->x1; i < placeholder->x2; i++)
			for (int j = placeholder->y1; j < placeholder->y2; j++) {
				int flipi = j - placeholder->y1 + placeholder->x1;
				int flipj = placeholder->y2 + placeholder->x1 - i - 1;
				placeholder->mat[flipi][flipj].grey = flip_pimg->mat[i][j].grey;
			}
		for (int i = placeholder->x1; i < placeholder->x2; i++)
			for (int j = placeholder->y1; j < placeholder->y2; j++)
				flip_pimg->mat[i][j].grey = placeholder->mat[i][j].grey;
	}
}

void filter_color(image *placeholder, image *filt_pimg,
				  float filter[3][3])
{
	int height1 = placeholder->x1;
	int height2 = placeholder->x2;
	int width1 = placeholder->y1;
	int width2 = placeholder->y2;
	if (height1 == 0)
		height1++;
	if (height2 == placeholder->height)
		height2--;
	if (width1 == 0)
		width1++;
	if (width2 == placeholder->width)
		width2--;
	for  (int i = height1; i < height2; i++) {
		for (int j = width1; j < width2; j++) {
			filt_pimg->mat[i][j].r = 0;
			filt_pimg->mat[i][j].g = 0;
			filt_pimg->mat[i][j].b = 0;
			for (int o = i - 1; o <= i + 1; o++)
				for (int k = j - 1; k <= j + 1; k++) {
					filt_pimg->mat[i][j].r += placeholder->mat[o][k].r *
											   filter[o - i + 1][k - j + 1];
					filt_pimg->mat[i][j].g += placeholder->mat[o][k].g *
											   filter[o - i + 1][k - j + 1];
					filt_pimg->mat[i][j].b += placeholder->mat[o][k].b *
											   filter[o - i + 1][k - j + 1];
				}
		}
	}
	for (int i = height1; i < height2; i++) {
		for (int j = width1; j < width2; j++) {
			placeholder->mat[i][j].r = filt_pimg->mat[i][j].r;
			placeholder->mat[i][j].g = filt_pimg->mat[i][j].g;
			placeholder->mat[i][j].b = filt_pimg->mat[i][j].b;
		}
	}
}

int clamp(float value, int mini, int maxi)
{
	if (value < mini)
		return mini;
	if (value > maxi)
		return maxi;
	return value;
}

void set_filter(float fltr1[3][3], float fltr2[3][3],
				float fltr3[3][3], float fltr4[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			if (i == 1 && j == 1)
				fltr1[i][j] = 8;
			else
				fltr1[i][j] = -1;
		}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j || i + j == 2) {
				fltr2[i][j] = 0;
				if (i == 1 && j == 1)
					fltr2[i][j] = 5;
			} else {
				fltr2[i][j] = -1;
			}
		}
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			fltr3[i][j] = 1;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			fltr3[i][j] /= 9;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == j || i + j == 2) {
				fltr4[i][j] = 1;
				if (i == 1 && j == 1)
					fltr4[i][j] = 4;
			} else {
				fltr4[i][j] = 2;
			}
		}
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			fltr4[i][j] /= 16;
}

void swap_numbers(int x1, int x2)
{
	int aux = x1;
	x1 = x2;
	x2 = aux;
}
