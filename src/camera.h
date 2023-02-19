#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
    vec3 position;
    vec3 direction;
    vec3 forward_velocity;
    float yaw;
} camera_t;

extern camera_t camera;

#endif 
