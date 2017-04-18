// drawing.h
// Header file for drawing.c
// Draw primitives by directly referencing the ST7735 driver
// Author: Sameer Bibikar

// Draw a circle using the ST7735 driver.
// The center of the circle is given by (x, y), and its radius by r.
// The color must be gotten from ST7735_Color565.
void drawing_circle(int16_t x, int16_t y, int16_t r, int16_t color);

// Draw a rectangle using the ST7735 driver,
// but also check that it is within the drawable region of the screen.
// The color must be gotten from ST7735_Color565.
void drawing_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t color);

// Draw a shooting star at the coordinates given by (x, y).
// To erase, just draw a rectangle over the old position.
// Automatically sets the color.
void drawing_star(int x, int y);

