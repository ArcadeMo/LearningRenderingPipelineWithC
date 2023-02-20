#include <math.h>
#include <stdint.h>
#include "display.h"
#include "triangle.h"
#include "swap.h"

/*

        (B)
        /|\
       / | \
      /  |  \
     /  (P)  \
    /  /   \  \
   / /       \ \
  //           \\
 (A)------------(C)

*/
// Return the barycentric weights alpha, beta, and gamma for point p
vec3 barycentric_weights(vec2 a, vec2 b, vec2 c, vec2 p) {
    // Find the vectors between the vertices ABC and point p
    vec2 ac = vec2_sub(c, a);
    vec2 ab = vec2_sub(b, a);
    vec2 ap = vec2_sub(p, a);
    vec2 pc = vec2_sub(c, p);
    vec2 pb = vec2_sub(b, p);

    // Compute the area of the full parallegram/triangle ABC using 2D cross product
    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); //  AC x AB 

    // Alpha is the area of the small parallelogram/triangle PBC divided by the area of the full parallelogram/triangle ABC
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

    // Beta is the area of the small parallelogram/triangle APC divided by the area of the full parallelogram/triangle ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

    // Weight gamma is easily found since barycentric coordinates always add up to 1.0
    float gamma = 1 - alpha - beta;

    vec3 weights = { alpha, beta, gamma };
    return weights;
}

/* Old flat bottom and flat top triangle fill
///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
//The top triangle
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) { //x2, y2 is actually the Mx, My    
    //Find the two slopes of each triangle legs
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0); //using the inverse slope because our increment is the y-axis and not the x-axis, making the y the run. Drawing each line based on the y-axis.
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    //Start x_start and x_end from the top vertex
    float x_start = x0; //the left line to start drawing the line
    float x_end = x0; //the right line where we end up
    
    //Loop all the scanlines from bottom to top
    for (int y = y0; y <= y2; y++) {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}


///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////



//The bottom triangle
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
   //Find the two slopes of each triangle legs
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0); //using the inverse slope because our increment is the y-axis and not the x-axis, making the y the run. Drawing each line based on the y-axis.
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    //Start x_start and x_end from the bottom vertex
    float x_start = x2; //the left line to start drawing the line
    float x_end = x2; //the right line where we end up
    
    //Loop all te scanlines from bottom to top
    for (int y = y2; y >= y0; y--) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}
*/

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

//Function to draw the textured pixel at position x and y using interpolation
void draw_triangle_pixel (
    int x, int y, uint32_t color,
    vec4 point_a, vec4 point_b, vec4 point_c
) {
    //Create three vec2 to find the interpolation
    vec2 p = { x, y };
    vec2 a = vec4_to_vec2(point_a);
    vec2 b = vec4_to_vec2(point_b);
    vec2 c = vec4_to_vec2(point_c);

    vec3 weights = barycentric_weights(a, b, c, p);
    
    //Extracting x,y,z cause its the values created from the barycentric function
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    //Declare variable to store the interpolated values of 1/W of the current pixel
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    //Adjust the 1/2 so the pixels  that are closer to the camera actually have smaller values 
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    //Only draw the pixel if the depth value is less than the one previously stored in the buffer
    if(interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        //Fetching from the color to draw at the location x,y
        draw_pixel(x, y, color);

        //Update the z_buffer with the value of 1/w of the current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

//Function to draw the textured pixel at position x and y using interpolation
void draw_texel(
    int x, int y, uint32_t* texture,
    vec4 point_a, vec4 point_b, vec4 point_c,
    tex2 a_uv, tex2 b_uv, tex2 c_uv
) {
    vec2 p = { x, y };
    vec2 a = vec4_to_vec2(point_a);
    vec2 b = vec4_to_vec2(point_b);
    vec2 c = vec4_to_vec2(point_c);

    vec3 weights = barycentric_weights(a, b, c, p);
    
    //Extracting x,y,z cause its the values created from the barycentric function
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    //Declare variables to store the interpolated values of U,V,1/W of the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    //Perform the interpolation of U/W, V/W using barycentric weights and a factor of 1/w. Getting the exact value of the point for the texture to be inserted ex. uv (0.47, 0.22) using barycentric weights
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    //Interpolate the value of 1/2 for the pixel
    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    //Divde back both interpolated values by 1/w
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    //Mapping the UV coord to the full texture and width
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width; //Should test the values of the tex_x and y to precent a buffer overflow in the texture array
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height; //modulus is a hacky way to fix the fill concention problem

    //Adjust the 1/2 so the pixels  that are closer to the camera actually have smaller values 
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    //Only draw the pixel if the depth value is less than the one previously stored in the buffer
    if(interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        //Fetching from the texture array the texture we want to draw at the location x,y
        draw_pixel(x, y, texture[(texture_width * tex_y) + tex_x]);

        //Update the z_buffer with the value of 1/w of the current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

/*
Draw a filled triangle with the flat-top/flat-bottom method
We split the original triangle in two, half flat-bottom and half flat-top

         (x0,y0)
           / \
          /   \
         /     \
        /       \
       /         \
  (x1,y1)------(Mx,My)
      _           \
         _         \
            _       \
               _     \
                  \   \
                    _  \
                       _\
                          \
                        (x2,y2)

*/

void draw_filled_triangle (
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
) {
    //Sort vertices by y-coords ascending (y0 < y1 < y2)
    if (y0 > y1) { 
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y0 > y2) { 
        int_swap(&y0, &y2);
        int_swap(&x0, &x2);
        float_swap(&z0, &z2);
        float_swap(&w0, &w2);
    }
    if (y1 > y2) { 
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    } 

    //Create vector points and texture coords after sorting vertices
    vec4 point_a = { x0, y0, z0, w0 };
    vec4 point_b = { x1, y1, z1, w1 };
    vec4 point_c = { x2, y2, z2, w2 };

    //Render the upper part of the triangle(flat-bottom)
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1; //Review how we got this formula
            int x_end = x0 + (y - y0) * inv_slope_2;

            //Gurantee x start is to the left of x end
            if (x_end < x_start) {
                int_swap(&x_start, &x_end); 
            }

            for(int x = x_start; x < x_end; x++) {
                //Draw each pixel mapped to the texture
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }

    //Render the bottom part of the triangle(flat-top)
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2- y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1; //Review how we got this formula
            int x_end = x0 + (y - y0) * inv_slope_2;

            //Gurantee x start is to the left of x end
            if (x_end < x_start) {
                int_swap(&x_start, &x_end); 
            }

            for(int x = x_start; x < x_end; x++) {
                //Draw each pixel mapped to the texture
                draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
            }
        }
    }
}

/*Old function to draw the filled triangle 
// void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
//     //Sort the vertices by y-coordinate ascending(y0 < y1 < y2)
//     if (y0 > y1) { 
//         int_swap(&y0, &y1);
//         int_swap(&x0, &x1);
//     }
//     if (y0 > y2) { 
//         int_swap(&y0, &y2);
//         int_swap(&x0, &x2);
//     }
//     if (y1 > y2) { 
//         int_swap(&y1, &y2);
//         int_swap(&x1, &x2);
//     }
//     //Special cases where we do not need to fill things in twice
//     if (y1 == y2) { //if the triangle is a perfect top triangle 
//         fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
//     } else if (y0 == y1) { // if its a perfect top
//         fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
//     } else {
//         //Calculate the mid-point (Mx, My) using triangle similarity
//         int My = y1;
//         int Mx = ((float)((x2 - x0) * (y1- y0)) / (float)(y2 - y0)) + x0;
//         //Draw flat triangles
//         fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
//         fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
//     }
// }
*/

/*/
Draw a textured triangle based on a texture array of colors.
We split the original triangle in two, half flat-bottom and half flat-top.
       v0
       /\
      /  \
     /    \
    /      \
  v1--------\
    _        \
       _      \
          _    \
             _  \
                 \
                  \
                  v2
*/
//Loop the pixels of the triangles for rendering color from texture
void draw_textured_triangle ( 
    int x0, int y0, float z0, float w0, float u0, float v0, 
    int x1, int y1, float z1, float w1, float u1, float v1, 
    int x2, int y2, float z2, float w2, float u2, float v2, 
    uint32_t* texture
) 
{
    //Sort vertices by y-coords ascending (y0 < y1 < y2)
    if (y0 > y1) { 
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y0 > y2) { 
        int_swap(&y0, &y2);
        int_swap(&x0, &x2);
        float_swap(&z0, &z2);
        float_swap(&w0, &w2);
        float_swap(&u0, &u2);
        float_swap(&v0, &v2);
    }
    if (y1 > y2) { 
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    } 

    //Flip the V component to account for inverted UV coords (0,0) starting on top left
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    //Create vector points and texture coords after sorting vertices
    vec4 point_a = { x0, y0, z0, w0 };
    vec4 point_b = { x1, y1, z1, w1 };
    vec4 point_c = { x2, y2, z2, w2 };
    tex2 a_uv = { u0, v0 };
    tex2 b_uv = { u1, v1 };
    tex2 c_uv = { u2, v2 };

    //Render the upper part of the triangle(flat-bottom)
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1; //Review how we got this formula
            int x_end = x0 + (y - y0) * inv_slope_2;

            //Gurantee x start is to the left of x end
            if (x_end < x_start) {
                int_swap(&x_start, &x_end); 
            }

            for(int x = x_start; x < x_end; x++) {
                //Draw each pixel mapped to the texture
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    //Render the bottom part of the triangle(flat-top)
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2- y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1; //Review how we got this formula
            int x_end = x0 + (y - y0) * inv_slope_2;

            //Gurantee x start is to the left of x end
            if (x_end < x_start) {
                int_swap(&x_start, &x_end); 
            }

            for(int x = x_start; x < x_end; x++) {
                //Draw each pixel mapped to the texture
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
}
