#include <stdio.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

#define MAX_NUM_MESHES 10
static tMesh meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_mesh(char* obj_filename, char* png_filename, vec3 scale, vec3 translation, vec3 rotation) {
    //Load the OBJ file for the mesh
    load_mesh_obj_data(&meshes[mesh_count], obj_filename);

    //Load the PNG file for the mesh
    load_mesh_png_data(&meshes[mesh_count], png_filename);

    //Initialize scale, translation, rotation
    meshes[mesh_count].scale = scale;
    meshes[mesh_count].translate = translation;
    meshes[mesh_count].rotate = rotation;

    //Add the mesh to the array of meshes
    mesh_count++;
}

void load_mesh_obj_data(tMesh* mesh, char* obj_filename) {
    FILE* file; //pointer to the file
    file = fopen(obj_filename, "r"); //r means only read

    char line[1024]; //means each line will have no more than x characters

    tex2* texcoords = NULL;

    while (fgets(line, 1024, file)){ //load the string from line upto 1024 character from the file, will only read one line unless we add a while loop
        
        //Vertex info
        if (strncmp(line, "v ", 2) == 0) { 
            vec3 vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z); //parameters are what is the source, then the format, then each %f becomes a coord
            array_push(mesh -> vertices, vertex);
        }; 
        
        //Texture coordinate info
        if (strncmp(line, "vt ", 3) == 0) {//looks for a line that starts with vt and has 3 chars
            tex2 texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v); 
            array_push(texcoords, texcoord);
        }

        //Face info
        if (strncmp(line, "f ", 2) == 0) { //2 character compares "f" and " ", it returns 0 if true
        int vertex_indices[3];
        int texture_indices[3];
        int normal_indices[3];
        sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
            &vertex_indices[0], &texture_indices[0], &normal_indices[0],
            &vertex_indices[1], &texture_indices[1], &normal_indices[1],
            &vertex_indices[2], &texture_indices[2], &normal_indices[2]
        ); //parameters are what is the source, then the format, then each %f becomes a coord
        face face = {
            .a = vertex_indices[0] - 1, //since arrays start at 0 we subtract 1 
            .b = vertex_indices[1] - 1,
            .c = vertex_indices[2] - 1,
            .a_uv = texcoords[texture_indices[0] - 1],
            .b_uv = texcoords[texture_indices[1] - 1],
            .c_uv = texcoords[texture_indices[2] - 1],
            .color = 0xFF808080
        };
        array_push(mesh -> faces, face);
        }; 
    }; 
    array_free(texcoords);
}

void load_mesh_png_data(tMesh* mesh, char* png_filename) {
    upng_t* png_image = upng_new_from_file(png_filename);
    if (png_image != NULL) {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK) {
            mesh -> texture = png_image;
        }
    }
}

int get_num_meshes(void) {
    return mesh_count;
}

tMesh* get_mesh(int index) {
    return &meshes[index];
}

void free_meshes(void){
    for(int i = 0; i < mesh_count; i++){
        upng_free(meshes[i].texture);
        array_free(meshes[i].faces);
        array_free(meshes[i].vertices);
    }
}

// meshs tMesh = {
//     .vertices = NULL,
//     .faces = NULL,
//     .rotate = {0, 0, 0},
//     .scale = {1.0, 1.0, 1.0},
//     .translate = {0, 0, 0}
// };

//not dynamic lists
// vec3 cube_vertices[N_CUBE_VERTICES] = {
//     { .x = -1, .y = -1, .z = -1 }, //0
//     { .x = -1, .y =  1, .z = -1 }, //1
//     { .x =  1, .y =  1, .z = -1 }, //2
//     { .x =  1, .y = -1, .z = -1 }, //3
//     { .x =  1, .y =  1, .z =  1 }, //4
//     { .x =  1, .y = -1, .z =  1 }, //5
//     { .x = -1, .y =  1, .z =  1 }, //6
//     { .x = -1, .y = -1, .z =  1 }  //7
// };

//The Top Left UV Coord is (0,0)
// face cube_faces[N_CUBE_FACES] = {
//     // front
//     { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
//     { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//     // right
//     { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
//     { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//     // back
//     { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
//     { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//     // left
//     { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
//     { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//     // top
//     { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
//     { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//     // bottom
//     { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
//     { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
// };

// void load_cube_mesh_data(void){
//     for (int i = 0; i < N_CUBE_VERTICES; i++){
//         vec3 cube_vertex = cube_vertices[i]; //loading all the vec3 data above and pushing
//         array_push(mesh.vertices, cube_vertex);
//     }
//     for (int i = 0; i < N_CUBE_FACES; i++){
//         face cube_face = cube_faces[i];
//         array_push(mesh.faces, cube_face); //same thing with the faces
//     }
// };

// void load_obj_file_data(char* filename){
//     FILE* file; //pointer to the file
//     file = fopen(filename, "r"); //r means only read

//     char line[1024]; //means each line will have no more than x characters

//     tex2* texcoords = NULL;

//     while (fgets(line, 1024, file)){ //load the string from line upto 1024 character from the file, will only read one line unless we add a while loop
        
//         //Vertex info
//         if (strncmp(line, "v ", 2) == 0) { 
//             vec3 vertex;
//             sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z); //parameters are what is the source, then the format, then each %f becomes a coord
//             array_push(mesh.vertices, vertex);
//         }; 
        
//         //Texture coordinate info
//         if (strncmp(line, "vt ", 3) == 0) {//looks for a line that starts with vt and has 3 chars
//             tex2 texcoord;
//             sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v); 
//             array_push(texcoords, texcoord);
//         }

//         //Face info
//         if (strncmp(line, "f ", 2) == 0) { //2 character compares "f" and " ", it returns 0 if true
//         int vertex_indices[3];
//         int texture_indices[3];
//         int normal_indices[3];
//         sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
//             &vertex_indices[0], &texture_indices[0], &normal_indices[0],
//             &vertex_indices[1], &texture_indices[1], &normal_indices[1],
//             &vertex_indices[2], &texture_indices[2], &normal_indices[2]
//         ); //parameters are what is the source, then the format, then each %f becomes a coord
//         face face = {
//             .a = vertex_indices[0] - 1, //since arrays start at 0 we subtract 1 
//             .b = vertex_indices[1] - 1,
//             .c = vertex_indices[2] - 1,
//             .a_uv = texcoords[texture_indices[0] - 1],
//             .b_uv = texcoords[texture_indices[1] - 1],
//             .c_uv = texcoords[texture_indices[2] - 1],
//             .color = 0xFF808080
//         };
//         array_push(mesh.faces, face);
//         }; 
//     }; 
//     array_free(texcoords);
// }

//NOTES
//%f is a float, %d decimal, %s string