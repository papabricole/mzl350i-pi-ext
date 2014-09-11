#ifndef ILI9481_H
#define ILI9481_H

#include <stdint.h>

#define RGB565(r,g,b)((r >> 3) << 11 | (g >> 2) << 5 | ( b >> 3))

int ili9481_init();
void ili9481_setwindow(uint32_t xs, uint32_t xe, uint32_t ys, uint32_t ye);
void ili9481_clear(uint16_t p);
void ili9481_pset(int dx,int dy, uint16_t color);
void ili9481_writedata(uint16_t val);
void ili9481_writedatan(uint32_t len, const uint16_t* data);

#endif // ILI9481_H
