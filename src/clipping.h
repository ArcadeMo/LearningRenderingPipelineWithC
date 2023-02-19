#ifndef CLIPPING_H
#define CLIPPING_H

#include "triangle.h"
#include "vector.h"

#define MAX_NUM_POLYGON_VERTICES 10
#define MAX_NUM_POLYGON_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

float float_lerp(float a, float b, float t);

typedef struct {
    vec3 point;
    vec3 normal;
} plane;

typedef struct {
    vec3 vertices[MAX_NUM_POLYGON_VERTICES];
    tex2 texcoords[MAX_NUM_POLYGON_VERTICES];
    int num_vertices; //counter of the number of vertices in current polygon
} tPolygon;


void init_frustum_planes(float fovx, float fovy, float znear, float zfar);

tPolygon polygon_from_triangle(vec3 v0, vec3 v1, vec3 v2, tex2 t0, tex2 t1, tex2 t2);

void triangles_from_polygon(tPolygon* polygon, triangle triangles[], int* num_triangles);
void clip_polygon_against_plane(tPolygon* polygon, int plane);
void clip_polygon(tPolygon* polygon); //out paramter, returns the function back into memory for polygon*

#endif 
