#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

typedef struct {
    int a;
    int b;
    int c;
    tex2 a_uv;
    tex2 b_uv;
    tex2 c_uv;
    uint32_t color;
} face; // stores the index of every vertex

typedef struct {
    vec4 vertex[3]; //stores the coordinates of each vertex
    tex2 texcoords[3];
    uint32_t color;
    // float avg_depth;
} triangle;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void draw_triangle_pixel (
    int x, int y, uint32_t color,
    vec4 point_a, vec4 point_b, vec4 point_c
);

void draw_texel (
    int x, int y, uint32_t* texture,
    vec4 point_a, vec4 point_b, vec4 point_c,
    tex2 a_uv, tex2 b_uv, tex2 c_uv
);

void draw_filled_triangle (
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
);

void draw_textured_triangle ( 
    int x0, int y0, float z0, float w0, float u0, float v0, 
    int x1, int y1, float z1, float w1, float u1, float v1, 
    int x2, int y2, float z2, float w2, float u2, float v2, 
    uint32_t* texture
);

#endif 
