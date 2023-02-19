#include "draw.h"
#include "colorBuffer.h"

void draw_pixel(int x, int y, uint32_t color){
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) { // setting restrains so we dont access memory inside the color buffer
        color_buffer[(window_width * y) + x] = color; // printing a color on a specific location, it would be window width (the number of rows on the screen)
    }
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

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}