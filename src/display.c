#include "display.h"

// define the global variables to something in the .c implementation file
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static float* z_buffer = NULL;

static uint32_t* color_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;

static int window_width;
static int window_height;

static int render_method = 0;
static int cull_method = 0;

//The only way to fetch a static variable
int get_window_width(void) {
    return window_width;
}

int get_window_height(void) {
    return window_height;
}

void set_render_method(int method) {
    render_method = method;
}

void set_cull_method(int method) {
    cull_method = method;
}

bool is_cull_backface(void) {
    return cull_method == CULL_BACKFACE;
}

bool check_to_render_filled_triangle(void) {
    return (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE);
}

bool check_to_render_textured_triangle(void) {
    return (render_method == RENDER_TEXTURE || render_method == RENDER_TEXTURE_WIRE);
}

bool check_to_render_wireframe(void) {
    return (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURE_WIRE);
}

bool check_to_render_wire_vertex(void) {
    return (render_method == RENDER_WIRE_VERTEX);
}

// in C for no parameters gotta write void
bool initialize_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { //we are checking for errors here with the !0 since no errors will return 0, sdl_init starts counting progressing miliseconds
        fprintf(stderr, "Error with initializing SDL. \n");
        return false;
    }

    //SDL query to determine the fullscreen width and height but this is a fake fullscreen, borderless windowed? 
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode); // display index which tells us what display its gettings, 0 in this case means the main display and a pointer to the SDL display mode we want to populate
    
    window_width = display_mode.w; // to change the resolution but fit to full screen you can make window_width/height = number, the current code gives display full resolution 
    window_height = display_mode.h;

    //Create and SDL window
    window = SDL_CreateWindow(
        NULL, //first parameter is a border
        SDL_WINDOWPOS_CENTERED, // posX position from the top left of the screen
        SDL_WINDOWPOS_CENTERED, // posy position from the top left of the screen
        window_width, //width
        window_height, //height
        SDL_WINDOW_BORDERLESS //flags like shadows, borders, etc
    );
    if (!window) { //checking if the pointer made contact to create a window with the shit above
        fprintf(stderr, "Error with creating the SDL window. \n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0); //first parameter is what is the pointer to the window it belongs to, second is what is the device (-1 is the default), 3rd is a bunch of flags (0 is no special flags)
    if (!renderer) { //checking if the pointer made contact to create a window with the shit above
        fprintf(stderr, "Error with creating the renderer bruh. \n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); //makes it actual fullscreen and not borderless windowed fullscreen 

    //Allocate the memory to hold for the color buffer (bytes)
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height); //malloc is allocating memory in the heap expected in bytes, if successful it returns a pointer to the first position of the block
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);
    //Creating a SDL texture to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer, // created that renderer with SDL 
        SDL_PIXELFORMAT_RGBA32, //ARGB8888, // the format of the pixel where its ARGB and its 8 bits each
        SDL_TEXTUREACCESS_STREAMING, // contiously streaming this frame by frame, access mode given to that texture
        window_width, // texture width and height
        window_height
    );

    return true;
}

void clear_z_buffer(void) {
    for (int i = 0; i < window_height * window_height; i++) { //one for loop rather than nested below, 0(n)
        z_buffer[i] = 1.0;
    // for (int y = 0; y < window_height; y++) {
    //     for (int x = 0; x < window_width; x++) {
    //             z_buffer[(window_width * y) + x] = 1.0;
    //     }
    }
}

void destroy_window(void) { //c doesnt have garbage collector so we are clearing all the memory allocating for previous functions
    free(color_buffer);
    free(z_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); //the memory freeing of SDL_Init
}


// the purpose of this function is to copy the color buffer and its content to the texture so it can render it 
void render_color_buffer(void) { 
    SDL_UpdateTexture(
        color_buffer_texture, //texture we want, we created it above
        NULL, //parameter giving you the option to divide the texture in rectangles
        color_buffer, // what is the source, what we are copying to our texture
        (int)(window_width * sizeof(uint32_t)) // texture pitch or the size of each row, how many bytes for each row
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL); // actually displaying the texture now and rendering it, 
    SDL_RenderPresent(renderer);
}

void clear_color_buffer(uint32_t color) { // this function just changes the mem and doesn't actually render
    for (int i = 0; i < window_height * window_height; i++) { //one for loop rather than nested below, 0(n)
        color_buffer[i] = color;
        // for (int x = 0; x < window_width; x++) {
        //         color_buffer[(window_width * y) + x] = color;
        // }
    }
}

// void draw_grid(void) {
//     for (int y = 0; y < window_height; y++) {
//         for (int x = 0; x < window_width; x++) {
//             if (x % 10 == 0 || y % 10 == 0){
//                 color_buffer[(window_width * y) + x] = 0xFFFFFF;
//             }
//         }
//     }
// }

// a little more efficient by only checking for x and y being divisible by 10 once per row or column, rather than for every pixel in the row or column
void draw_grid(void) {
    for (int y = 0; y < window_height; y += 75) { //This loop iterates over the y coordinate of the pixels in the window, starting from 0 and incrementing by 100 for each iteration, until y is equal to or greater than window_height
        for (int x = 0; x < window_width; x+= 25) { // or (int x = 0; x < window_width; x++): This inner loop iterates over the x coordinate of the pixels in the window, starting from 0 and incrementing by 1 for each iteration, until x is equal to or greater than window_width
            color_buffer[(window_width * y) + x] = 0xFFFFFF; //color_buffer[(window_width * y) + x] = 0xFFFFFF: For each x and y combination, this line sets the color of the corresponding pixel in the color_buffer to white (0xFFFFFF in hexadecimal).
        }
    }
    for (int x = 0; x < window_width; x += 75) {
        for (int y = 0; y < window_height; y+= 25) { // +=100 makes it into dots rather than grid
            color_buffer[(window_width * y) + x] = 0xFFFFFF;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) { // setting restrains so we dont access memory inside the color buffer
            return;
    }
    color_buffer[(window_width * y) + x] = color; // printing a color on a specific location, it would be window width (the number of rows on the screen)
}


void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = (x1 - x0); //slope
    int delta_y = (y1- y0);

    int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y); //a single line if statement
    
    float x_inc = delta_x / (float) longest_side_length; //increment the x on the grid or the y based on the longest delta
    float y_inc = delta_y / (float) longest_side_length;

    float current_x = x0; //the starting point to draw the line
    float current_y = y0;

    //Loop to draw the pixel while we increment from the starting point
    for (int i = 0; i < longest_side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

// draw rect
// The first for loop has the variable i, which starts at 0 and goes up to width minus 1.
// The second for loop has the variable j, which starts at 0 and goes up to height minus 1.
// In each iteration of the inner loop, current_x is calculated as x + i and current_y is calculated as y + j.
// The color of the pixel at the current current_x and current_y position is set to color by accessing the color_buffer array at the index (window_width * current_y) + current_x.
// This process is repeated for all values of i and j, resulting in a rectangle being drawn on the color_buffer

void draw_rect(int x, int y, int width, int height, uint32_t color) { // x and y is where it starts from the left top position of the screen, width and height is how many pixels in the width and height
    for (int i = 0; i < width; i++) { // loop through all of the width length, going from 0 to width
        for (int j = 0; j < height; j++) { //loop through all of height length
            int current_x = x + i; // original x + the increment loop
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
            // color_buffer[(window_width * current_y) + current_x] = color; // just color all of the currents
        }
    }
}

float get_zbuffer_at(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0;
    }
    return z_buffer[(window_width * y) + x];
}

void update_zbuffer_at(int x, int y, float value) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    z_buffer[(window_width * y) + x] = value;
}
