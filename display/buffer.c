#include <stdint.h>
#include "ST7735.h"
#include "drawing.h"

#include "../input/portf.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 160
#define BUFFER_WIDTH (DISPLAY_WIDTH+1)
#define BUFFER_HEIGHT (DISPLAY_HEIGHT+1)

// The actual display buffer.
// Note that while the colors transmitted to the ST7735 are 16-bit
// per pixel, we don't have enough space in RAM for that, so instead,
// we will store 8 bits per pixel at the cost of reduced color
// depth.
uint8_t buffer[BUFFER_WIDTH*BUFFER_HEIGHT];
// All the pixels are just stored consecutively.
// To get a pixel at (x, y), the formula is
// buffer[x + y*DISPLAY_WIDTH].

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void buffer_clear() {
	volatile uint8_t i, j, k;
//	DisableInterrupts();
	for (i = 0; i <= DISPLAY_WIDTH; ) {
		portf_toggle(1);
		for (j = 0; j <= DISPLAY_HEIGHT;) {
			portf_toggle(2);
			k = j*BUFFER_WIDTH;
			k += i;
			j++;
			buffer[k] = 0;
			portf_toggle(3);
		}
		i++;
	}
//	EnableInterrupts();
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
	uint8_t colorval = (red & 0xE0) | ((green & 0xE0) >> 3) | ((blue & 0xC0) >> 6);
	return colorval;
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

uint16_t buffer_displaycolor(uint8_t color) {
	return ST7735_Color565(buffer_red(color), buffer_green(color), buffer_blue(color));
}

void buffer_fill(uint8_t color) {
//	DisableInterrupts();
	for (volatile int i = 0; i <= DISPLAY_WIDTH; i++) {
		for (volatile int j = 0; j <= DISPLAY_HEIGHT; j++) {
			buffer[i+j*BUFFER_WIDTH] = color;
		}
	}
//	EnableInterrupts();
}

// Draw a circle into the buffer.
// Only parts of the circle which are on the screen will be drawn.
// Inputs: int x - the X coordinate of the center of the circle
// int y - the Y coordinate of the center of the circle
// int r - the radius of the circle
void buffer_circle(int16_t x, int16_t y, int16_t r, uint8_t color) {
	int i = drawing_max(x-r, 0);
	int xEnd = drawing_min(x+r, DISPLAY_WIDTH);
	int j = drawing_max(y-r, 0);
	int yEnd = drawing_min(y+r, DISPLAY_HEIGHT);
	for (; i < xEnd; i++) {
		for (; j < yEnd; j++) {
			if ((i-x)*(i-x)+(j-y)*(j-y) <= r*r) {
				buffer[i+j*DISPLAY_WIDTH] = color;
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
	int xEnd = drawing_min(x+w, DISPLAY_WIDTH);
	int j = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, DISPLAY_HEIGHT);
	for (; i <= xEnd; i++) {
		for (; j <= yEnd; j++) {
			buffer[i+j*DISPLAY_WIDTH] = color;
		}
	}
} 

// Draw the outline of a rectangle using the ST7735 driver.
// Inputs: int x - X coordinate of top left corner
// int y - Y coordinate of top left corner
// int w - width of rectangle
// int h - height of rectangle
void buffer_rect_outline(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
	int xStart = drawing_max(x, 0);
	int xEnd = drawing_min(x+w, DISPLAY_WIDTH);
	int yStart = drawing_max(y, 0);
	int yEnd = drawing_min(y+h, DISPLAY_HEIGHT);
	for (int i = xStart; i < xEnd; i++) {
		buffer[i+yStart*DISPLAY_WIDTH] = color;
		buffer[i+yEnd*DISPLAY_WIDTH] = color;
	}

	for (int j = yStart; j < yEnd; j++) {
		buffer[xStart+j*DISPLAY_WIDTH] = color;
		buffer[xEnd+j*DISPLAY_WIDTH] = color;
	}
}

// Draw a shooting star at (x, y).
// To erase, just draw a rectangle over the old position.
void buffer_star(int16_t x, int16_t y) {
	if (x > ST7735_TFTWIDTH || x < 0) return;
	if (y + 5 > ST7735_TFTHEIGHT || y < 0) return;
	uint8_t grey_value = 32;
	for (int i = 0; i < 5; i++) {
		buffer[x+(y+i)*DISPLAY_WIDTH] = buffer_color(grey_value, grey_value, grey_value);
		grey_value += 32;
	}
}

void buffer_write() {
//	DisableInterrupts();
	ST7735_SetWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	for (volatile int i = DISPLAY_HEIGHT; i > 0;) {
		for (volatile int j = DISPLAY_WIDTH; j > 0;) {
			ST7735_PushColor(buffer_displaycolor(buffer[j+i*DISPLAY_WIDTH]));
			j--;
		}
		i--;
	}
//	EnableInterrupts();
}
