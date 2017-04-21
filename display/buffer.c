#include <stdint.h>
#include "ST7735.h"
#include "drawing.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 160

// The actual display buffer.
// Note that while the colors transmitted to the ST7735 are 16-bit
// per pixel, we don't have enough space in RAM for that, so instead,
// we will store 8 bits per pixel at the cost of reduced color
// depth.
uint8_t buffer[128][160];

void buffer_clear() {
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			buffer[i][j] = 0;
		}
	}
}

// Pack the three color values into one.
// Results in a significant loss of color depth.
// Bits 7:5 of the return value are bits 7:5 of red.
// Bits 4:2 of the return value are bits 7:5 of green.
// Bits 1:0 of the return value are bits 7:6 of blue.
uint8_t buffer_color(uint8_t red, uint8_t green, uint8_t blue) {
	// We will give red and green 3 bits each, and 2 bits for
	// blue. This choice is purely arbitrary, so it may change
	// later.
	
	return ((red >> 5) << 5) + ((green >> 5) << 2) + (blue >> 6);
}

// Gets the red color value packed into the color
uint8_t buffer_red(uint8_t color) {
	return (color >> 5) << 5;
}

// Gets the green color value packed into the color
uint8_t buffer_green(uint8_t color) {
	return (color >> 2) << 5;
}

// Gets the blue color value packed into the color
uint8_t buffer_blue(uint8_t color) {
	return color << 6;
}

void buffer_fill(uint8_t color) {
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			buffer[i][j] = color;
		}
	}
}

// Draw a circle into the buffer.
// Only parts of the circle which are on the screen will be drawn.
// Inputs: int x - the X coordinate of the center of the circle
// int y - the Y coordinate of the center of the circle
// int r - the radius of the circle
void buffer_circle(int16_t x, int16_t y, int16_t r, int16_t color) {
	int i = drawing_max(x-r, 0);
	int xEnd = drawing_min(x+r, ST7735_TFTWIDTH);
	int j = drawing_max(y-r, 0);
	int yEnd = drawing_min(y+r, ST7735_TFTHEIGHT);
	for (; i < xEnd; i++) {
		for (; j < yEnd; j++) {
			if ((i-x)*(i-x)+(j-y)*(j-y) <= r*r) {
				buffer[i][j] = color;
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
void buffer_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
	int i = drawing_max(x, 0);
	int xEnd = drawing_min(x+w, ST7735_TFTWIDTH);
	int j = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, ST7735_TFTHEIGHT);
	for (; i <= xEnd; i++) {
		for (; j <= yEnd; j++) {
			buffer[i][j] = color;
		}
	}
} 

// Draw the outline of a rectangle using the ST7735 driver.
// Inputs: int x - X coordinate of top left corner
// int y - Y coordinate of top left corner
// int w - width of rectangle
// int h - height of rectangle
void buffer_rect_outline(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color) {
	int xStart = drawing_max(x, 0);
	int xEnd = drawing_min(x+w, ST7735_TFTWIDTH);
	int yStart = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, ST7735_TFTHEIGHT);
	for (int i = xStart; i < xEnd; i++) {
		buffer[i][yStart] = color;
		buffer[i][yEnd] = color;
	}

	for (int j = yStart; j < yEnd; j++) {
		buffer[xStart][j] = color;
		buffer[xEnd][j] = color;
	}
}

// Draw a shooting star at (x, y).
// To erase, just draw a rectangle over the old position.
void buffer_star(int16_t x, int16_t y) {
	uint8_t grey_value = 32;
	for (int i = 0; i < 5; i++) {
		buffer[x][y+i] = buffer_color(grey_value, grey_value, grey_value);
		grey_value += 32;
	}
}

