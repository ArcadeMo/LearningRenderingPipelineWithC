#include "vector.h"
#include <math.h>

//Implementation of vector 2 operations
vec2 vec2_new(float x, float y) {
    vec2 result = { x , y };
    return result;
}

float vec2_length(vec2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2 vec2_add(vec2 a, vec2 b) { 
    vec2 result = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return result;
}

vec2 vec2_sub(vec2 a, vec2 b) { 
    vec2 result = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return result;
}

vec2 vec2_mul(vec2 v, float s) {
    vec2 result = {
        .x = v.x * s,
        .y = v.y * s
    };
    return result;
}

vec2 vec2_div(vec2 v, float s) {
    vec2 result = {
        .x = v.x / s,
        .y = v.y / s
    };
    return result;
}

float vec2_dot(vec2 a, vec2 b) {
    return (a.x * b.x) + (a.y * b.y);
}

void vec2_normalize(vec2* v) { 
    float length = sqrt(v->x * v->x + v->y * v->y); //if we need to access a variable inside a structure that is a pointer we need to use arrow notation
    v->x /= length; //dont need to return anything because we are changing the variable directly through arrow notation
    v->y /= length;
}

//Implementation of vector 3 operations
vec3 vec3_new(float x, float y, float z) {
    vec3 result = { x , y, z };
    return result;
}

vec3 vec3_clone(vec3* v) {
    vec3 result = { v->x, v->y, v->z };
    return result;
}

float vec3_length(vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 vec3_add(vec3 a, vec3 b) { 
    vec3 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return result;
}

vec3 vec3_sub(vec3 a, vec3 b) { 
    vec3 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
    return result;
}

vec3 vec3_mul(vec3 v, float s) {
    vec3 result = {
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s
    };
    return result;
}

vec3 vec3_div(vec3 v, float s) {
    vec3 result = {
        .x = v.x / s,
        .y = v.y / s,
        .z = v.z / s
    };
    return result;
}

vec3 vec3_cross(vec3 a, vec3 b) {
    vec3 result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
    return result;
}

float vec3_dot(vec3 a, vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

void vec3_normalize(vec3* v) { 
    float length = sqrt(v->x * v->x + v->y * v->y + v->z * v->z); //if we need to access a variable inside a structure that is a pointer we need to use arrow notation
    v->x /= length; //dont need to return anything because we are changing the variable directly through arrow notation
    v->y /= length;
    v->z /= length;
}

vec3 vec3_rotate_x(vec3 v, float angle) {
    vec3 rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

vec3 vec3_rotate_y(vec3 v, float angle) {
    vec3 rotated_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

vec3 vec3_rotate_z(vec3 v, float angle) {
    vec3 rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}

//Vector conversion
vec4 vec3_to_vec4(vec3 v) {
    vec4 result = { v.x, v.y, v.z, 1.0 };
    return result;
}

vec3 vec4_to_vec3(vec4 v) {
    vec3 result = { v.x, v.y, v.z };
    return result;
}

vec2 vec4_to_vec2(vec4 v) {
    vec2 result = { v.x, v.y };
    return result;
}