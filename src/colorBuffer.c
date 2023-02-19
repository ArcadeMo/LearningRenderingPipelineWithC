#include "colorBuffer.h"

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

// the purpose of this function is to copy the color buffer and its content to the texture so it can render it 
void render_color_buffer(void) { 
    SDL_UpdateTexture(
        color_buffer_texture, //texture we want, we created it above
        NULL, //parameter giving you the option to divide the texture in rectangles
        color_buffer, // what is the source, what we are copying to our texture
        (int)(window_width * sizeof(uint32_t)) // texture pitch or the size of each row, how many bytes for each row
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL); // actually displaying the texture now and rendering it, 
}

void clear_color_buffer(uint32_t color) { // this function just changes the mem and doesn't actually render
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
                color_buffer[(window_width * y) + x] = color;
        }
    }

}