#include "display.h"

// define the global variables to something in the .c implementation file
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
float* z_buffer = NULL;

int window_width;
int window_height;

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

    return true;
}

void clear_z_buffer(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
                z_buffer[(window_width * y) + x] = 1.0;
        }
    }
}

void destroy_window(void) { //c doesnt have garbage collector so we are clearing all the memory allocating for previous functions
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); //the memory freeing of SDL_Init
}
