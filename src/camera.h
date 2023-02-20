#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct {
    vec3 position;
    vec3 direction;
    vec3 forward_velocity;
    float yaw;
    float pitch;
} tCamera;

// extern camera_t camera;

void init_camera(vec3 position, vec3 direction);

vec3 get_camera_position(void);
vec3 get_camera_direction(void);
vec3 get_camera_forward_velocity(void);

float get_camera_yaw(void);
float get_camera_pitch(void);

void update_camera_position(vec3 position);
void update_camera_direction(vec3 direction);
void update_camera_forward_velocity(vec3 forward_velocity);

void rotate_camera_yaw(float angle);
void rotate_camera_pitch(float angle);

vec3 get_camera_lookat_target(void);

#endif 
