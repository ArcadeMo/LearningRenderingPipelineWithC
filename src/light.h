#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"

//Only a direction since its a global light coming from one direction
typedef struct {
    vec3 direction;
} light;

extern light global_light;

uint32_t light_affected_intensity(uint32_t original_color, float percentage);

#endif 
