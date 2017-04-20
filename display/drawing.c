// drawing.c
// Draw primitives by directly referencing the ST7735 driver
// Author: Sameer Bibikar

#include <stdlib.h>
#include <stdint.h>
#include "ST7735.h"

// Get the maximum of two integers.
static int drawing_max(int a, int b) {
	if (a >= b) return a;
	return b;
}

// Get the minimum of two integers.
static int drawing_min(int a, int b) {
	if (a < b) return a;
	return b;
}

// Draw a circle using the ST7735 driver.
// Only parts of the circle which are on the screen will be drawn.
// Inputs: int x - the X coordinate of the center of the circle
// int y - the Y coordinate of the center of the circle
// int r - the radius of the circle
void drawing_circle(int16_t x, int16_t y, int16_t r, int16_t color) {
	int xStart = drawing_max(x-r, 0);
	int xEnd = drawing_min(x+r, ST7735_TFTWIDTH);
	int yStart = drawing_max(y-r, 0);
	int yEnd = drawing_min(y+r, ST7735_TFTHEIGHT);
	for (int i = xStart; i < xEnd; i++) {
		for (int j = yStart; j < yEnd; j++) {
			if ((i-x)*(i-x)+(j-y)*(j-y) <= r*r) {
				ST7735_DrawPixel(i, j, color);
			}
		}
	}
}

// Draw a rectangle using the ST7735 driver.
// Only parts of the rectangle on the screen will be drawn.
// Inputs: int x - X coordinate of top left corner
// int y - Y coordinate of top left corner
// int w - width of rectangle
// int h - height of rectangle
void drawing_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color) {
	int xStart = drawing_max(x, 0);
	int xEnd = drawing_min(x+w, ST7735_TFTWIDTH);
	int yStart = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, ST7735_TFTHEIGHT);
	ST7735_FillRect(xStart, yStart, xEnd-xStart, yEnd-yStart, color);
} 

// Draw the outline of a rectangle using the ST7735 driver.
// Inputs: int x - X coordinate of top left corner
// int y - Y coordinate of top left corner
// int w - width of rectangle
// int h - height of rectangle
void drawing_rect_outline(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color) {
	ST7735_DrawFastVLine(x, y, h, color);
	ST7735_DrawFastVLine(x+w, y, h, color);
	ST7735_DrawFastHLine(x, y, w, color);
	ST7735_DrawFastHLine(x, y+h, w, color);
}

// Draw a shooting star at (x, y).
// To erase, just draw a rectangle over the old position.
void drawing_star(int16_t x, int16_t y) {
	uint8_t grey_value = 31;
	for (int i = 0; i < 5; i++) {
		ST7735_DrawPixel(x, y+i, ST7735_Color565(grey_value, grey_value, grey_value));
		grey_value += 32;
	}
}

