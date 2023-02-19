#include "matrix.h"
#include <math.h>


vec4 mat4_mul_vec4(mat4 m, vec4 v) {
    vec4 result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

mat4 mat4_mul_mat4(mat4 a, mat4 b) { 
    mat4 m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j]; //incrementing the rows of a by the incrementing the columns of b
        }
    }
    return m;
}

mat4 mat4_perspective(float fov, float aspect, float znear, float zfar) {
   /*  | (h/w)*1/tan(fov/2)            0         0                 0 |
       |                  0 1/tan(fov/2)         0                 0 |
       |                  0            0 zf/(zf-zn) (-zf*zn)/(zf-zn) |
       |                  0            0         1                 0 | */
       mat4 m = {{{ 0 }}}; //zero-matrix
       m.m[0][0] = aspect * (1 / tan(fov / 2));
       m.m[1][1] = 1 / tan(fov / 2);
       m.m[2][2] = zfar / (zfar - znear);
       m.m[2][3] = (-zfar * znear) / (zfar - znear); //the z offset 
       m.m[3][2] = 1.0; //save orginal z in w
       return m;
}

vec4 mat4_mul_vec4_project(mat4 mat_projection, vec4 v) { 
    //Multiply the projection matrix by the original vector
    vec4 result = mat4_mul_vec4(mat_projection, v);

    //Perform perspective divide with original z-value stored in w
    if (result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    return result;
}

mat4 mat_eye(void) {
    /* | 1 0 0 0 |
       | 0 1 0 0 |
       | 0 0 1 0 |
       | 0 0 0 1 | */
       mat4 m = {{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
       }};
       return m;
}

mat4 mat_scale(float sx, float sy, float sz) {
    /* |sx  0  0 0 |
       | 0 sy  0 0 |
       | 0  0 sz 0 |
       | 0  0  0 1 | */
       mat4 m = mat_eye();
       m.m[0][0] = sx;
       m.m[1][1] = sy;
       m.m[2][2] = sz;
       return m;
}

mat4 mat_translate(float tx, float ty, float tz) {
    /* | 1 0 0 tx |
       | 0 1 0 ty |
       | 0 0 1 tz |
       | 0 0 0  1 | */
       mat4 m = mat_eye();
       m.m[0][3] = tx;
       m.m[1][3] = ty;
       m.m[2][3] = tz;
       return m;
}

mat4 mat_rotate_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    /* | 1 0  0 0 |
       | 0 c -s 0 |
       | 0 s  c 0 |
       | 0 0  0 1 | */
       mat4 m = mat_eye();
       m.m[1][1] = c;
       m.m[1][2] = -s;
       m.m[2][1] = s;
       m.m[2][2] = c;
       return m;
}

mat4 mat_rotate_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    /* |  c 0 s 0 |
       |  0 1 0 0 |
       | -s 0 c 0 |
       |  0 0 0 1 | */
       mat4 m = mat_eye();
       m.m[0][0] = c;
       m.m[0][2] = s;
       m.m[2][0] = -s;
       m.m[2][2] = c;
       return m;
}

mat4 mat_rotate_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    /* | c -s 0 0 |
       | s  c 0 0 |
       | 0  0 0 0 |
       | 0  0 0 1 | */
       mat4 m = mat_eye();
       m.m[0][0] = c;
       m.m[0][1] = -s;
       m.m[1][0] = s;
       m.m[1][1] = c;
       return m;
}

mat4 mat4_look_at(vec3 pin, vec3 target, vec3 up) {
    // Compute the forward (z), right (x), and up (y) vectors
    vec3 z = vec3_sub(target, pin);
    vec3_normalize(&z);
    vec3 x = vec3_cross(up, z);
    vec3_normalize(&x);
    vec3 y = vec3_cross(z, x);

    // | x.x   x.y   x.z  -dot(x,pin) |
    // | y.x   y.y   y.z  -dot(y,pin) |
    // | z.x   z.y   z.z  -dot(z,pin) |
    // |   0     0     0            1 |
    mat4 view_matrix = {{
        { x.x, x.y, x.z, -vec3_dot(x, pin) },
        { y.x, y.y, y.z, -vec3_dot(y, pin) },
        { z.x, z.y, z.z, -vec3_dot(z, pin) },
        {   0,   0,   0,                 1 }
    }};
    return view_matrix;
}