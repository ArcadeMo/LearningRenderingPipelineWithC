#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS) // how many milliseconds each of one of those frames should take to complete 60 fps  

//enums allow to create a list of options
enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURE,
    RENDER_TEXTURE_WIRE
} render_method;

// extern makes it declare as global variables 
extern SDL_Window* window;
extern SDL_Renderer* renderer;

// extern uint32_t* color_buffer;
// extern SDL_Texture* color_buffer_texture;

extern int window_width;
extern int window_height;

bool initialize_window(void);

void clear_z_buffer(void);

void destroy_window(void);

extern float* z_buffer;

#endif