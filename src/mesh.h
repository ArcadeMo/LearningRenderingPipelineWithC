#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

//not dynamics arrays
// #define N_CUBE_VERTICES 8
// #define N_CUBE_FACES (6 * 2)

// extern vec3 cube_vertices[N_CUBE_VERTICES];
// extern face cube_faces[N_CUBE_FACES];  //6 cube faces, 2 triangle faces per side
// extern tMesh mesh;

//Define a struct dynamic size meshes with the array of vertices and faces
typedef struct {
    vec3* vertices; //dynamic array of vertices
    face* faces;    //dynamic array of faces
    upng_t* texture; //a pointer for each mesh's own texture, mesh PNG texture
    vec3 rotate;  //rotation of x,y,z
    vec3 scale;     //scale with x,y,z
    vec3 translate; //translating with x,y,z
} tMesh;

void load_mesh(char* obj_filename, char* png_filename, vec3 scale, vec3 translation, vec3 rotation);

void load_mesh_obj_data(tMesh* mesh, char* obj_filename);

void load_mesh_png_data(tMesh* mesh, char* png_filename);

int get_num_meshes(void);

tMesh* get_mesh(int index);

void free_meshes(void);

// void load_obj_file_data(char* filename);
// void load_cube_mesh_data(void);

//NOTES
//vertices and faces is a vec3 pointer since it needs to be dynamic since its a pointer to the first position and then start allocating more if needed
//meshes are basically game objects in this case
#endif 