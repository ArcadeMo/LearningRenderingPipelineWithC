#ifndef COLORBUFFER_H
#define COLORBUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "display.h"

extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);

#endif