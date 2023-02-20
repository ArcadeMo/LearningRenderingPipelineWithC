#include <math.h>
#include "clipping.h"

#define NUM_PLANES 6

plane frustum_planes[NUM_PLANES];

/*
Frustum planes are defined by a point and a normal vector

Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))


          /|\
        /  | | 
      /\   | |
    /      | |
 P|-->  <-||   ----> +z-axis
    \      | |
      /   |  |
        \  | | 
          |/

*/
void init_frustum_planes(float fovx, float fovy, float znear, float zfar) {
    float cos_half_fovx = cos(fovx / 2);
    float sin_half_fovx = sin(fovx / 2);
    float cos_half_fovy = cos(fovy / 2);
    float sin_half_fovy = sin(fovy / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fovx;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fovx;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fovy;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fovy;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, znear);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, zfar);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

tPolygon polygon_from_triangle(vec3 v0, vec3 v1, vec3 v2, tex2 t0, tex2 t1, tex2 t2) {
    tPolygon polygon = {
        .vertices = { v0, v1, v2 },
        .texcoords = { t0, t1, t2 },
        .num_vertices = 3
    };
    return polygon;
}

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void clip_polygon_against_plane(tPolygon* polygon, int plane) { //int because enum is stored as an int
    vec3 plane_point = frustum_planes[plane].point;
    vec3 plane_norm = frustum_planes[plane].normal;

    //Declare a static array of inside vertives that will be part of the final polygon return
    vec3 inside_vertices[MAX_NUM_POLYGON_VERTICES];
    tex2 inside_texcoords[MAX_NUM_POLYGON_VERTICES];
    int num_inside_vertices = 0;

    //Declare the current vertex and previous vertex as a pointer starting from the first in the array 
    vec3* current_vertex = &polygon -> vertices[0];
    vec3* previous_vertex = &polygon -> vertices[polygon -> num_vertices - 1];

    //Pointer to the uvcoords for textures
    tex2* current_texcoord = &polygon -> texcoords[0];
    tex2* previous_texcoord = &polygon -> texcoords[polygon -> num_vertices - 1];

    //Declare the dot products of the current and previous 
    float current_dot = 0;
    float previous_dot = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_norm);

    //Loop while the current vertex is different from the ones stored in the array (can do a hash for this)
    while (current_vertex != &polygon -> vertices[polygon ->num_vertices]) {
        current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_norm);

        //Check if the vertex switched from inside to outside or vice versa
        if(current_dot * previous_dot < 0) {
            //Calculate the interpolation factor t
            float t = previous_dot / (previous_dot - current_dot);

            //Calculate the intersection point I = Q1 + t(Q2-Q1)
            vec3 intersection_point = {
                .x = float_lerp(previous_vertex -> x, current_vertex -> x, t),
                .y = float_lerp(previous_vertex -> y, current_vertex -> y, t),
                .z = float_lerp(previous_vertex -> z, current_vertex -> z, t)
            };

            //  = vec3_clone(current_vertex); //I = Qc
            // intersection_point = vec3_sub(intersection_point, *previous_vertex); //I = Qc - Qp
            // intersection_point = vec3_mul(intersection_point, t); // I = t(Qc - Qp)
            // intersection_point = vec3_add(intersection_point, *previous_vertex); //I = Q1 + (Q2-Q1)

            //Use lerp formula to get the interpolated U and V texcoords
            tex2 interpolated_texcoord = {
                .u = float_lerp(previous_texcoord -> u, current_texcoord -> u, t),
                .v = float_lerp(previous_texcoord -> v, current_texcoord -> v, t),
            };

            //Insert the intersection point to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            inside_texcoords[num_inside_vertices] = tex2_clone(&interpolated_texcoord);
            num_inside_vertices++;
        }


        //Checks if the current dot is postive and if so the vertex point is inside the plane
        if (current_dot > 0){
            //Add the current vertex to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
            num_inside_vertices++;

        }

        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
        current_vertex++;
        current_texcoord++;
    }
    
    //Copy the list of the inside vertices inside the destination polygon (out paramter tPolygon polygon*)
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon -> vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon -> texcoords[i] = tex2_clone(&inside_texcoords[i]);
    }
    polygon -> num_vertices = num_inside_vertices;
}

void triangles_from_polygon(tPolygon* polygon, triangle triangles[], int* num_triangles) { //static arrays do not need to be passed by reference
    for (int i = 0; i < polygon -> num_vertices - 2; i++) {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].vertex[0] = vec3_to_vec4(polygon -> vertices[index0]);
        triangles[i].vertex[1] = vec3_to_vec4(polygon -> vertices[index1]);
        triangles[i].vertex[2] = vec3_to_vec4(polygon -> vertices[index2]);

        triangles[i].texcoords[0] = (polygon -> texcoords[index0]);
        triangles[i].texcoords[1] = (polygon -> texcoords[index1]);
        triangles[i].texcoords[2] = (polygon -> texcoords[index2]);
    }
    *num_triangles = polygon -> num_vertices - 2;
}


void clip_polygon(tPolygon* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE); //each new clipped polygon is being sent to the function below it to be further clipped
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

