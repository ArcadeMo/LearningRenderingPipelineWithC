#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} mat4;

vec4 mat4_mul_vec4(mat4 m, vec4 v);
mat4 mat4_mul_mat4(mat4 a, mat4 b);

//Projection
mat4 mat4_perspective(float fov, float aspect, float znear, float zfar);

vec4 mat4_mul_vec4_project(mat4 mat_projection, vec4 v);

//Function to return the identity matrix
mat4 mat_eye(void);

//Scale matrix
mat4 mat_scale(float sx, float sy, float sz);

//Trnslate matrix
mat4 mat_translate(float tx, float ty, float tz);

//Rotate matrix
mat4 mat_rotate_x(float angle);
mat4 mat_rotate_y(float angle);
mat4 mat_rotate_z(float angle);

//Camera matrix
mat4 mat4_look_at(vec3 pin, vec3 target, vec3 up);

#endif 
