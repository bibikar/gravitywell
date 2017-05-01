#include <stdint.h>
#include "ST7735.h"
#include "drawing.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 160
#define BUFFER_WIDTH (DISPLAY_WIDTH+1)
#define BUFFER_HEIGHT (DISPLAY_HEIGHT+1)

// The actual display buffer.
// Note that while the colors transmitted to the ST7735 are 16-bit
// per pixel, we don't have enough space in RAM for that, so instead,
// we will store 8 bits per pixel at the cost of reduced color
// depth.
uint8_t buffer[BUFFER_WIDTH][BUFFER_HEIGHT];

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void buffer_clear() {
	for (uint8_t i = 0; i <= DISPLAY_WIDTH; i++) {
		for (uint8_t j = 0; j <= DISPLAY_HEIGHT; j++) {
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
	for (uint8_t i = 0; i <= DISPLAY_WIDTH; i++) {
		for (uint8_t j = 0; j <= DISPLAY_HEIGHT; j++) {
			buffer[i][j] = color;
		}
	}
}

// Draw a circle into the buffer.
// Only parts of the circle which are on the screen will be drawn.
// Inputs: int x - the X coordinate of the top left corner of the circle
// int y - the Y coordinate of the top left corner of the circle
// int r - the radius of the circle
void buffer_circle(int16_t x, int16_t y, int16_t r, uint8_t color) {
	int xStart = drawing_max(x, 0);
	int xEnd = drawing_min(x+2*r, DISPLAY_WIDTH);
	int yStart = drawing_max(y, 0);
	int yEnd = drawing_min(y+2*r, DISPLAY_HEIGHT);
	x = x+r;
	y = y+r;
	for (int i = xStart; i < xEnd; i++) {
		for (int j = yStart; j < yEnd; j++) {
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
	uint8_t i = drawing_max(x, 0);
	uint8_t xEnd = drawing_min(x+w, DISPLAY_WIDTH);
	uint8_t j = drawing_max(y, 0);
	uint8_t yEnd = drawing_min(y+h, DISPLAY_HEIGHT);
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
void buffer_rect_outline(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
	uint8_t xStart = drawing_max(x, 0);
	uint8_t xEnd = drawing_min(x+w, DISPLAY_WIDTH);
	uint8_t yStart = drawing_max(y, 0);
	uint8_t yEnd = drawing_min(y+h, DISPLAY_HEIGHT);
	for (uint8_t i = xStart; i <= xEnd; i++) {
		buffer[i][yStart] = color;
		buffer[i][yEnd] = color;
	}

	for (uint8_t j = yStart; j < yEnd; j++) {
		buffer[xStart][j] = color;
		buffer[xEnd][j] = color;
	}
}

// Draw a shooting star at (x, y).
// To erase, just draw a rectangle over the old position.
void buffer_star(int16_t x, int16_t y) {
	if (x > ST7735_TFTWIDTH || x < 0) return;
	if (y + 5 > ST7735_TFTHEIGHT || y < 0) return;
	uint8_t grey_value = 32;
	for (int i = 0; i < 5; i++) {
		buffer[x][y+i] = buffer_color(grey_value, grey_value, grey_value);
		grey_value += 32;
	}
}


// Draw a missile at (x, y).
// To erase, just draw a rectangle over the old position.
void buffer_missile(int16_t x, int16_t y) {
	if (x > ST7735_TFTWIDTH || x < 0) return;
	if (y + 10 > ST7735_TFTHEIGHT || y < 0) return;
	uint8_t grey_value = 16;
	for (int i = 0; i < 10; i++) {
		buffer[x][y+i] = buffer_color(255-grey_value, 0, 0);
		grey_value += 16;
	}
}


// Draw a character into the buffer.
// Adapted from ST7735.c
void buffer_char(int16_t x, int16_t y, char c, uint8_t color) {
	uint8_t line; // vertical column of pixels in font
	int32_t i, j;

	// Clip character
	if (x < 0) return;
	if (y < 0) return;
	if (x + 5 > ST7735_TFTWIDTH) return;
	if (y + 8 > ST7735_TFTHEIGHT) return;
	
	// Actually draw the character
	for (i = 0; i < 6; i++) {
		// There are 6 columns
		if (i == 5) line = 0x0; // Last column is empty
		else line = ST7735_GetFont((c*5)+i);
		// Now for the rows:
		for (j = 0; j < 8; j++) {
			if (line & 0x1) { // If the current pixel needs to be filled
				buffer[x+i][y+j] = color;
			} 
			line >>= 1;
		}
	}
}

// Draw a string into the buffer, using buffer_char.
void buffer_string(int16_t x, int16_t y, char *string, uint8_t color) {
	int16_t x_orig = x;
	while (*string) {
		buffer_char(x, y, *string, color);
		string++;
		x += 6;  
		if (x + 5 >= ST7735_TFTWIDTH || *string == '\n') {
			y += 10; // wrap
			x = x_orig;
		}
		if (y + 8 > ST7735_TFTHEIGHT) return;
	}
}


void buffer_write() {
	ST7735_SetWindow(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	for (uint8_t i = 0; i < DISPLAY_HEIGHT; i++) {
		for (uint8_t j = 0; j < DISPLAY_WIDTH; j++) {
			ST7735_PushColor(buffer_displaycolor(buffer[j][i]));
		}
	}
}


const unsigned short PlayerShip0[] = {
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0,
 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};


void buffer_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const unsigned short *s)
{
	//to add the sprite to the buffer
	//x and y are the top left coordinates
	//w and h are the width and height of the buffer
	uint32_t i,j,size, count=0;
	uint8_t xStart = drawing_max(x, 0);
	uint8_t xEnd = drawing_min(x+w, DISPLAY_WIDTH);
	uint8_t yStart = drawing_max(y, 0);
	uint8_t yEnd = drawing_min(y+h, DISPLAY_HEIGHT);
	for(i=yStart;i<=yEnd;++i)
	{
		for(j=xStart;j<=xEnd;++j)
		{	if(s[count]!=0)
				buffer[j][i] = s[count];
			++count;
		}
	}
}
	
static void buffer_num_helper(int16_t *x, int16_t y, int32_t n, uint8_t color) {
	if (n/10 != 0) buffer_num_helper(x, y, n/10, color);
	buffer_char(*x, y, n%10+48, color);
	(*x)+=6;
}

void buffer_num(int16_t x, int16_t y, int32_t n, uint8_t color){
	buffer_num_helper(&x, y, n, color);
}
