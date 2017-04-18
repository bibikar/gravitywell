// drawing.c
// Draw primitives using a given function pointer to draw a pixel
// Author: Sameer Bibikar

#include <stdlib.h>
#include <stdint.h>

// This is a function pointer!
// Since I want to keep this code abstract right now, we will
// have some external thing set this to something,
// using the drawing_init function...
// Inputs: The first int is X coordinate on display
// Second int is Y coordinate on display.
void (*gDrawPixel)(int, int);

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
void drawing_init(void (*pixel_draw_func)(int, int), int w, int h) {
	gDrawPixel = pixel_draw_func;
	drawing_width = w;
	drawing_height = h;
	drawing_initialized_flag = 1;
}

// Draw a circle using the function pointer given
// Only parts of the circle which are on the screen will be drawn.
// Inputs: int x - the X coordinate of the center of the circle
// int y - the Y coordinate of the center of the circle
// int r - the radius of the circle
void drawing_circle(int x, int y, int r) {
	if (!drawing_initialized_flag) return;
	for (int i = 0; i < drawing_width; i++) {
		for (int j = 0; j < drawing_height; j++) {
			if ((i-x)*(i-x)+(j-y)*(j-y) <= r*r) {
				(*gDrawPixel)(i, j);
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
void drawing_rect(int x, int y, int w, int h) {
	if (!drawing_initialized_flag) return;
	int xStart = drawing_max(x, 0);
	int xEnd = drawing_min(x+w, drawing_width);
	int yStart = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, drawing_height);
	for (int i = xStart; i < xEnd; i++) {
		for (int j = yStart; j < yEnd; j++) {
			(*gDrawPixel)(i, j);
		}
	}
} 
