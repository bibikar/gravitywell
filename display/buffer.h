// buffer.h
// The header file for buffer.c, for buffering of the display in RAM.

void buffer_clear(void);
uint8_t buffer_color(uint8_t, uint8_t, uint8_t);
uint8_t buffer_red(uint8_t);
uint8_t buffer_green(uint8_t);
uint8_t buffer_blue(uint8_t);
void buffer_fill(uint8_t);
void buffer_circle(int16_t, int16_t, int16_t, int16_t);
void buffer_rect(int16_t, int16_t, int16_t, int16_t, int16_t);
void buffer_rect_outline(int16_t, int16_t, int16_t, int16_t, int16_t);
void buffer_star(int16_t, int16_t);
