#ifndef TEXTURE_H
#define TEXTURE_H

// #include <stdint.h>
// #include "upng.h"

typedef struct {
    float u;
    float v;
} tex2;

// extern int texture_width;
// extern int texture_height;

// extern const uint8_t REDBRICK_TEXTURE[];

// extern upng_t* png_texture;
// extern uint32_t* mesh_texture;

// void load_png_texture_data(char* filename);

tex2 tex2_clone(tex2* t);

#endif 
