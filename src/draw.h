#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "draw.h"

void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif
