#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

//not dynamics arrays
#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern vec3 cube_vertices[N_CUBE_VERTICES];
extern face cube_faces[N_CUBE_FACES];  //6 cube faces, 2 triangle faces per side

//Define a struct dynamic size meshes with the array of vertices and faces
typedef struct {
    vec3* vertices; //dynamic array of vertices
    face* faces;    //dynamic array of faces
    vec3 rotate;  //rotation of x,y,z
    vec3 scale;     //scalee with x,y,z
    vec3 translate; //translating with x,y,z

} meshs;

extern meshs mesh;

void load_cube_mesh_data(void);

void load_obj_file_data(char* filename);

//NOTES
//vertices and faces is a vec3 pointer since it needs to be dynamic since its a pointer to the first position and then start allocating more if needed

#endif 