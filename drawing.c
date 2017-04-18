// drawing.c
// Draw primitives using a given function pointer to draw a pixel
// Author: Sameer Bibikar

#include <stdlib.h>
#include <stdint.h>
#include "ST7735.h"

uint8_t drawing_initialized_flag = 0;

// Store the size of the screen
int drawing_width;
int drawing_height;

// Get the maximum of two integers.
int drawing_max(int a, int b) {
	if (a >= b) return a;
	return b;
}

// Get the minimum of two integers.
int drawing_min(int a, int b) {
	if (a < b) return a;
	return b;
}

// Set the pixel drawing function.
// It should be defined elsewhere and passed here before anything
// else in this file is called.
// Inputs: Pointer to function which takes two ints (coordinates of px)
// outputs nothing, and will draw the pixel to the screen
// There are also two ints you must input, which are the width and 
// height of the place we can draw onto.
void drawing_init(int w, int h) {
	drawing_width = w;
	drawing_height = h;
	drawing_initialized_flag = 1;
}

// Draw a circle using the function pointer given
// Only parts of the circle which are on the screen will be drawn.
// Inputs: int x - the X coordinate of the center of the circle
// int y - the Y coordinate of the center of the circle
// int r - the radius of the circle
void drawing_circle(int16_t x, int16_t y, int16_t r, int16_t color) {
	if (!drawing_initialized_flag) return;
	int xStart = drawing_max(x-r, 0);
	int xEnd = drawing_min(x+r, drawing_width);
	int yStart = drawing_max(y-r, 0);
	int yEnd = drawing_min(y+r, drawing_height);
	for (int i = xStart; i < xEnd; i++) {
		for (int j = yStart; j < yEnd; j++) {
			if ((i-x)*(i-x)+(j-y)*(j-y) <= r*r) {
				ST7735_DrawPixel(i, j, color);
			}
		}
	}
}

// Draw a rectangle using the function pointer given
// Only parts of the rectangle on the screen will be drawn.
// Inputs: int x - X coordinate of top left corner
// int y - Y coordinate of top left corner
// int w - width of rectangle
// int h - height of rectangle
void drawing_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color) {
	if (!drawing_initialized_flag) return;
	int xStart = drawing_max(x, 0);
	int xEnd = drawing_min(x+w, drawing_width);
	int yStart = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, drawing_height);
	ST7735_FillRect(xStart, yStart, xEnd-xStart, yEnd-yStart, color);
} 

// Draw a shooting star at (x, y).
// To erase, just draw a rectangle over the old position.
void drawing_star(int x, int y) {
	if (!drawing_initialized_flag) return;
	uint8_t grey_value = 31;
	for (int i = 0; i < 5; i++) {
		ST7735_DrawPixel(x, y+i, ST7735_Color565(grey_value, grey_value, grey_value));
		grey_value += 32;
	}
}

