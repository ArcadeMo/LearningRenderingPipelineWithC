#include <stdint.h>
#include "light.h"

static light global_light;

void init_light(vec3 direction) {
    global_light.direction = direction;
}

vec3 get_light_direction(void) {
    return global_light.direction;
}

//Change the color based on the light intensity percentage
uint32_t light_affected_intensity(uint32_t original_color, float percentage) {
    if (percentage < 0) percentage = 0;
    if (percentage > 1) percentage = 1;

    //seperating the channels using a bitwise operation to mask the color, only keeping the FFs
    uint32_t a = (original_color & 0xFF000000); 
    uint32_t r = (original_color & 0x00FF0000) * percentage;
    uint32_t g = (original_color & 0x0000FF00) * percentage;
    uint32_t b = (original_color & 0x000000FF) * percentage;

    uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return new_color;
}