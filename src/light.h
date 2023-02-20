#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"

//Only a direction since its a global light coming from one direction
typedef struct {
    vec3 direction;
} light;

// extern light global_light;
void init_light(vec3 direction);
vec3 get_light_direction(void);

uint32_t light_affected_intensity(uint32_t original_color, float percentage);

#endif 
