#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} vec2;

typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

//Vector 2D functions
vec2 vec2_new(float x, float y);

float vec2_length(vec2 v);

vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_mul(vec2 v, float s);
vec2 vec2_div(vec2 v, float s);
float vec2_dot(vec2 a, vec2 b);
void vec2_normalize(vec2* v); //we are testing a reference so we need to pass a pointer

//Vector 3D functions
vec3 vec3_new(float x, float y, float z);
vec3 vec3_clone(vec3* v);

float vec3_length(vec3 v);

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_mul(vec3 v, float s);
vec3 vec3_div(vec3 v, float s);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_dot(vec3 a, vec3 b);
void vec3_normalize(vec3* v);

//Manipulating the vectors 
vec3 vec3_rotate_x(vec3 v, float angle);
vec3 vec3_rotate_y(vec3 v, float angle);
vec3 vec3_rotate_z(vec3 v, float angle);

//Vector conversion function
vec4 vec3_to_vec4(vec3 v);
vec3 vec4_to_vec3(vec4 v);
vec2 vec4_to_vec2(vec4 v);

#endif