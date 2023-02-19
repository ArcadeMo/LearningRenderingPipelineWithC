#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "array.h"
#include "display.h"
#include "draw.h"
#include "colorBuffer.h"
#include "vector.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "matrix.h"
#include "light.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"

//Declare an array of vectors or points 
// const int N_POINTS = 9 * 9 * 9; // for a 9x9 cube
// vec3 cube_points[N_POINTS]; 
// vec2 projected_points[N_POINTS]; 

#define MAX_TRIANGLES_PER_MESH 25000
triangle triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

// Expensive way of rendering triangles
// triangle* triangles_to_render = NULL; //it is a pointer to a memory address that is gonna hold the type triangles

//Global Variables
bool is_running = false;
int previous_frame_time = 0;
float delta_time;

// vec3 camera_position = { 0, 0, 0 };
// vec3 cube_rotation = {.x = 0, .y = 0, .z = 0};

mat4 world_matrix;
mat4 projection_matrix;
mat4 view_matrix;

// float fov_factor = 640; //distance between each dot

//No parameters gotta write void
void setup(void) {
    //Intialize culling and render mode methods
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

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

    //Intialize the perspective projection matrix 
    float aspectx = (float)window_width / (float)window_height; // aspect ratio
    float aspecty = (float)window_height / (float)window_width; // aspect ratio
    
    float fovy = M_PI / 3.0; // 180/3 radians on unit cricle or 60 degrees
    float fovx = atan(tan(fovy/2) * aspectx) * 2.0;
    float znear = 1.0;
    float zfar = 25.0;
    projection_matrix = mat4_perspective(fovy, aspecty, znear, zfar);

    //Intialize frustrum planes with a point and a unit normal
    init_frustum_planes(fovx, fovy, znear, zfar);

    //Manually loading the hardcoded tedture data from the static array
    // mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    // texture_width = 64;
    // texture_height = 64;

    //Loads the values in the mesh data structure
    // load_cube_mesh_data();
    load_obj_file_data("./assets/f117.obj");

    //Load texture information from an external PNG file
    load_png_texture_data("./assets/f117.png");

    //Counter for each point used inside the next function
    // int point_counter = 0;

    //Start loading the array of vectors 
    // From -1 to 1 (in the 9x9 cube)
    // for (float x = -1; x <= 1; x += 0.25) { // loop to get the points for a cube from a grid -1 to 1 and 0.25 increments so 9 points in each line
    //     for (float y = -1; y <= 1; y += 0.25) {
    //         for (float z = -1; z <= 1; z += 0.25) {
    //             vec3 new_point = { .x = x, .y = y, .z = z }; // .x is the struct compenet recieves x, basically naming the points inside
    //             cube_points[point_counter++] = new_point; // incrementing 
    //         }
    //     }
    // }

    //Vector math tests
    // vec3 a = { 10.0, -12.0, 95.0 };
    // vec3 b = { -2.0, 27.0, -96.0 };

    // float a_length = vec3_length(a);
    // float b_length = vec3_length(b);
    // vec3 add_ab = vec3_add(a, b);
}           

void process_input(void) {
    SDL_Event event; 
    SDL_PollEvent(&event); //passing the reference of the event we created above so we can check the event being created using the switch case below

    switch (event.type){
        case SDL_QUIT: //when you click the X button on top then it SDL_QUIT is run
            is_running = false;  //if SDL at any time quits then is_running becomes false 
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) //escape key pressed
                is_running = false;
            if (event.key.keysym.sym == SDLK_1)
                render_method = RENDER_WIRE_VERTEX;
            if (event.key.keysym.sym == SDLK_2)
                render_method = RENDER_WIRE;
            if (event.key.keysym.sym == SDLK_3)
                render_method = RENDER_FILL_TRIANGLE;
            if (event.key.keysym.sym == SDLK_4)
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            if (event.key.keysym.sym == SDLK_5)
                render_method = RENDER_TEXTURE;
            if (event.key.keysym.sym == SDLK_6)
                render_method = RENDER_TEXTURE_WIRE;
            if (event.key.keysym.sym == SDLK_c)
                cull_method = CULL_BACKFACE;
            if (event.key.keysym.sym == SDLK_v)
                cull_method = CULL_NONE;
            if (event.key.keysym.sym == SDLK_UP)
                camera.position.y += 3.0 * delta_time;
            if (event.key.keysym.sym == SDLK_DOWN)
                camera.position.y -= 3.0 * delta_time;
            if (event.key.keysym.sym == SDLK_a)
                camera.yaw -= 1.0 * delta_time;
            if (event.key.keysym.sym == SDLK_d)
                camera.yaw += 1.0 * delta_time;
            if (event.key.keysym.sym == SDLK_w) {
                camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time); //scaling and calculating current camera velocity
                camera.position = vec3_add(camera.position, camera.forward_velocity);
            }
            if (event.key.keysym.sym == SDLK_s) {
                camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time); //scaling and calculating current camera velocity
                camera.position = vec3_sub(camera.position, camera.forward_velocity);
            }       
            break;
        default:
            break;
    }
}

// // function that recieves a 3D vector and returns a projected 2D point
// vec2 project(vec3 point) {
//     vec2 projected_point = {
//         .x = (fov_factor * point.x) / point.z, // multiply the x and y by the fov_factor to scale distance between each dot
//         .y = (fov_factor * point.y) / point.z // divide by z to apply the rule of perspective divide
//     };
//     return projected_point;
// }

void update(void) {
    //Delay function
    //Time to wait is storing how many miliseconds it should sleep calculated below
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait); // how many miliseconds we want to delay 
    }
    
    // it is better not to use a while loop since it is a processer instructor function meaning that the cpu will go 100% when running this task, delay function is better since it is built to give attention to other processes using cores
    // while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME)); //locking the execution and when it reaches the frame target time you release the execution

    //Get a change of time factor converted to seconds to update the objects
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0; 

    previous_frame_time = SDL_GetTicks(); // returning how many miliseconds has passed since the game started
    
    //Intialize the array of triangles to render to make sure every update we start an empty array
    // triangles_to_render = NULL;
    
    //Intialize and reset the counter of numbers to render for the current frame
    num_triangles_to_render = 0;

    //Change the mesh scale, rotation, and translation per animation frame 
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.002;

    mesh.rotate.x -= 0.00 * delta_time;
    mesh.rotate.y += 0.00 * delta_time;// for each update or each frame the y rotation is increasing my .1
    mesh.rotate.z += 0.00 * delta_time;

    // mesh.translate.x += 0.1 * delta_time; //change per second when using delta time as compared to per frame

    //Change the camera position per animation frame
    // camera.position.x += 0.5 * delta_time;
    // camera.position.y -= 0.8 * delta_time;

    //Translate the vertex away from camera(pushing everything inside the monitor 5 units)
    mesh.translate.z = 5.0;

    //Create a scale/translate/rotate matrix to multiply the mesh vertices
    mat4 scale_matrix = mat_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4 translate_matrix = mat_translate(mesh.translate.x, mesh.translate.y, mesh.translate.z);
    mat4 rotate_matrix_x = mat_rotate_x(mesh.rotate.x);
    mat4 rotate_matrix_y = mat_rotate_y(mesh.rotate.y);
    mat4 rotate_matrix_z = mat_rotate_z(mesh.rotate.z);

    //Hardcoded target point 
    // vec3 target = { 0, 0, 5.0 };

    //Initialize the target point at the positive z-axis
    vec3 target = { 0, 0, 1 };
    mat4 camera_yaw_rotation = mat_rotate_y(camera.yaw);
    camera.direction = vec4_to_vec3(mat4_mul_vec4(camera_yaw_rotation, vec3_to_vec4(target)));

    //Offset the camera position to the direction the camera is pointing at
    target = vec3_add(camera.position, camera.direction);

    //Default up vector is normalized y
    vec3 up = { 0, 1, 0 };

    //Create the view matrix (it is in the update function because it needs to be updated frame by frame related to the target)
    view_matrix = mat4_look_at(camera.position, target, up); 

    //Loop the triangle faces of the cube
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        // if (i != 4) continue; //only renders the top triangle in the front facing side of the cube for clipping testing

        face mesh_face = mesh.faces[i]; //mesh face is the current mesh face in the loop

        //Getting the vertices which are vectors pointing that vertex
        vec3 face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a]; 
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        //Create an array to save te transformed vertex in lines 137-139
        vec4 transformed_vertices[3];

        //Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4 transformed_vertex = vec3_to_vec4(face_vertices[j]); //got the orginal face vertices at 0-2

            //Create a World Matrix combining scale, rotation, and translation matrices
            world_matrix = mat_eye();

            //Muliply all transformation matrices and load the world matrix > order matters: scale, rotate, translate 
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotate_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotate_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotate_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(translate_matrix, world_matrix);

            //Multiply the world matrix by the orginal vector to place the object into world space
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            //Multiply the view matrix by the transformed vertex into camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            //Seperate matrix calculations
            // //Use a matrix to scale the vertex
            // transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);
            // //Rotate
            // transformed_vertex = mat4_mul_vec4(rotate_matrix_x, transformed_vertex);
            // transformed_vertex = mat4_mul_vec4(rotate_matrix_y, transformed_vertex);
            // transformed_vertex = mat4_mul_vec4(rotate_matrix_z, transformed_vertex);
            // //Translate
            // transformed_vertex = mat4_mul_vec4(translate_matrix, transformed_vertex);

            //Manual rotation 
            // transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            // transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            // transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            //Save transformed vertex into an array
            transformed_vertices[j] = transformed_vertex;
        }
        
        //Backface culling test to check if the current face should be projected
        vec3 vector_a = vec4_to_vec3(transformed_vertices[0]);  /*   A   */
        vec3 vector_b = vec4_to_vec3(transformed_vertices[1]);  /*  / \  */
        vec3 vector_c = vec4_to_vec3(transformed_vertices[2]);  /* C---B */

        //Vector subtraction
        vec3 vector_ab = vec3_sub(vector_b, vector_a);
        vec3 vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        //Compute face normal using cross product
        vec3 normal = vec3_cross(vector_ab, vector_ac); //since this engine is using a left handed coord system this is the order of the cross product we use
        vec3_normalize(&normal); //passing the reference of the address that is holding normal and that means whatever i do with the function, it changes the variable to the new outcome

        //Find the vector between a point and the camera origin
        vec3 origin = { 0, 0, 0 };
        vec3 camera_ray = vec3_sub(origin, vector_a);

        //Calculate alignment of the camera ray and face normal using the dot product
        float camera_alignment = vec3_dot(normal, camera_ray);

        //Bypass the triangles that are looking away from the camera
        if (cull_method == CULL_BACKFACE) {
            if (camera_alignment < 0) {
                continue; //simply continues executing this block if the check passes
            } 
        }

        //Before projecting we must clip

        //Create a polygon from the original triangle to be clipped
        tPolygon polygon = polygon_from_triangle (
            vec4_to_vec3(transformed_vertices[0]), 
            vec4_to_vec3(transformed_vertices[1]), 
            vec4_to_vec3(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
        );

        //Clip the polygon and return a new polygon with possible new vertices 
        clip_polygon(&polygon); //passing it by reference so we can change it directly

        //Break the polygon into triangles
        triangle triangles_after_clipping[MAX_NUM_POLYGON_TRIANGLES];
        int num_triangles_after_clipping = 0;

        triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

        //Loop through all assembled triangles after clipping
        for (int t = 0; t < num_triangles_after_clipping; t++) {
            triangle triangle_after_clipping = triangles_after_clipping[t];

            vec4 projected_points[3];

            //Loop all three vertices to perfrom the projection
            for (int j = 0; j < 3; j++) {
                //Project the current vertex
                // projected_points[j] = project(vec4_to_vec3(transformed_vertices[j]));

                projected_points[j] = mat4_mul_vec4_project(projection_matrix, triangle_after_clipping.vertex[j]);

                //Invert the y values to account for flipped screen y coordinates
                projected_points[j].y *= -1;

                //Scale into the view
                projected_points[j].x *= (window_width / 2.0);
                projected_points[j].y *= (window_height / 2.0);


                //Translate the project vertex to the middle of the screen 
                projected_points[j].x += (window_width / 2.0);
                projected_points[j].y += (window_height / 2.0);
            }
            
            // //Calculate the average depth for each face based on the transformed vertices
            // float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

            //Calculate the shade intenstity using the dot product of the face normal and the inverse of the light ray
            float light_intensity = -vec3_dot(normal, global_light.direction);

            //Calculate the triangle color based on the light angle
            uint32_t triangle_color = light_affected_intensity(mesh_face.color, light_intensity);

            triangle triangle_to_render = {
                .vertex = {
                    { projected_points[0].x, projected_points[0].y,  projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y,  projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y,  projected_points[2].z, projected_points[2].w }
                },
                .texcoords = {
                    { triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v },
                    { triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v },
                    { triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v }
                },
                .color = triangle_color,
                // .avg_depth = avg_depth 
            };

            //Save the projected triangle in the array of triangles to render
            //triangles_to_render[i] = projected_triangle;

            //pushing the triangles one by one dynamically, the cube model only has 12 triangles but other complex models will need more
            //This is expensive performance wise because we allocate and free constantly 
            // array_push(triangles_to_render, projected_triangle);

            //Static array one by one
            if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                //Inexpensive way to project the triangle in the stack
                triangles_to_render[num_triangles_to_render++] = triangle_to_render;
            }
        }
    }

    //Bubble sort the triangles to render by their avg depth
    // int num_triangles = array_length(triangles_to_render);
    // for (int i = 0; i < num_triangles; i++ ) {
    //     for (int j = 0; j < num_triangles; j++ ) {
    //         if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
    //             //Swap the tirangles positions
    //             triangle temp = triangles_to_render[i];
    //             triangles_to_render[i] = triangles_to_render[j];
    //             triangles_to_render[j] = temp;
    //         }
    //     }
    // }
    // for loop to project the cube properly
    // for (int i = 0; i < N_POINTS; i++) {
    //     vec3 point = cube_points[i]; // the vec3 point is the cube point of each i
    //     vec3 transformed_point = vec3_rotate_y(point, cube_rotation.y);
    //     transformed_point = vec3_rotate_x(transformed_point, cube_rotation.x);
    //     transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);
    //     // Translated the point away from the camera
    //     transformed_point.z -= camera_position.z; 
    //     //project the current point
    //     vec2 projected_point = project(transformed_point); // we have a projected point that comes from the project point from above 
    //     //project 
    //     projected_points[i] = projected_point; // saving or array manipulation of storginf the array we created in the global with the vec2 points created equal to the projected points that is calculated by the project function
    // }
}

void render(void) {
    SDL_RenderClear(renderer);

    draw_grid();

    // Loop through all projected points and render them 
    // for (int i = 0; i < N_POINTS; i++) {
    //     vec2 projected_point = projected_points[i];
    //     // instead of pixel we draw a small rectangle to see the end of the vector
    //     draw_rect(
    //         projected_point.x + (window_width / 2), //transforming to the middle of the screen
    //         projected_point.y + (window_height / 2),
    //         3,
    //         3,
    //         0xFFFF0000
    //     );
    // }
    
    /* Expensive way how to figure out how many triangles are needed to render
    // int num_triangles = array_length(triangles_to_render);

    //Loop through all projected triangles and render them 
    for (int i = 0; i < num_triangles; i++) {
        triangle triangle = triangles_to_render[i];
    */

    //Loop through all projected triangles and render them 
    for (int i = 0; i < num_triangles_to_render; i++) {
        triangle triangle = triangles_to_render[i];
        
        //Draw filled triangles
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(
                triangle.vertex[0].x, triangle.vertex[0].y, triangle.vertex[0].z, triangle.vertex[0].w, //vertex A
                triangle.vertex[1].x, triangle.vertex[1].y, triangle.vertex[1].z, triangle.vertex[1].w, //vertex B
                triangle.vertex[2].x, triangle.vertex[2].y, triangle.vertex[2].z, triangle.vertex[2].w, //vertex C
                triangle.color
            );
        }

        //Draw textured triangle
        if (render_method == RENDER_TEXTURE || render_method == RENDER_TEXTURE_WIRE) {
            draw_textured_triangle(
                triangle.vertex[0].x, triangle.vertex[0].y, triangle.vertex[0].z, triangle.vertex[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.vertex[1].x, triangle.vertex[1].y, triangle.vertex[1].z, triangle.vertex[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.vertex[2].x, triangle.vertex[2].y, triangle.vertex[2].z, triangle.vertex[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                mesh_texture
            );
        }
        
        //Draw the triangle wireframe
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURE_WIRE) {
            draw_triangle(
                triangle.vertex[0].x, triangle.vertex[0].y,
                triangle.vertex[1].x, triangle.vertex[1].y,
                triangle.vertex[2].x, triangle.vertex[2].y,
                0xFFFFFFF
            );
        }

        //Draw triangle vertex points
        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(
                triangle.vertex[0].x - 3, triangle.vertex[0].y - 3,
                6,
                6,
                0xFFFF0000
            );
            draw_rect(
                triangle.vertex[1].x - 3, triangle.vertex[1].y - 3,
                6,
                6,
                0xFFFF0000
            );
            draw_rect(
                triangle.vertex[2].x - 3, triangle.vertex[2].y - 3,
                6,
                6,
                0xFFFF0000
            );
        }
    }

    // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFFFF0000);

    // draw_line(100, 200, 300, 50, 0xFFFF6600);
    // draw_pixel(50, 50, 0xFFA020F0);
    // draw_rect(500, 500, 301, 201, 0xFFFFA500);

    //clear the array of triangles to render every frame loop ALWAYS GOT TO REMEMBER TO FREE MEMORY!!!
    // array_free(triangles_to_render);

    //First render the color buffer
    render_color_buffer();

    // going to loop through all the positions in memory and set it to that color 
    clear_color_buffer(0xFF000000); 

    //Empty the z_buffer after rendering
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

//Free resources
void free_resources(void) {
    free(color_buffer);
    free(z_buffer);
    upng_free(png_texture);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int main(int argc, char* args[]){
    // create an SDL window
    is_running = initialize_window(); //checks if true and false

    setup();

    while(is_running){      // using the boolean of the window being running 
        process_input();    // very niave game loop because it relies on the processor, faster the processor the faster it performs
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}


//NOTES 

//game loop render things frame by frame 
//if malloc fails to allocate the amount(maybe the amchine does not have enough memory) it will return a null pointer
//calloc() sets
//better programming you would add a check for color buffer
//if(!color buffer){
//  the return of malloc is a NULL pointer so the allocation was not successful
//     else blah blah blah
// }

//sizeof is not a function it is an operator so you can technically can write sizeof int and not sizeof(int)

//allocating dynamic memory inside a game loop is a bad idea since it can be super slow