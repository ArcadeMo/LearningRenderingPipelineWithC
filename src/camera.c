#include "camera.h"

static tCamera camera;

// camera_t camera = {
//     .position = { 0, 0, 0 },
//     .direction = { 0, 0, 1 },
//     .forward_velocity = { 0, 0, 0 },
//     .yaw = 0.0
// };

void init_camera(vec3 position, vec3 direction) {
    camera.position = position;
    camera.direction = direction;
    camera.forward_velocity = vec3_new(0, 0, 0);
    camera.yaw = 0.0;
    camera.pitch = 0.0;
};

vec3 get_camera_position(void) {
    return camera.position;
}

vec3 get_camera_direction(void) {
    return camera.direction;
}

vec3 get_camera_forward_velocity(void) {
    return camera.forward_velocity;
}

float get_camera_yaw(void) {
    return camera.yaw;
}

float get_camera_pitch(void) {
    return camera.pitch;
}

void update_camera_position(vec3 position) {
    camera.position = position;
}
void update_camera_direction(vec3 direction) {
    camera.direction = direction;
}
void update_camera_forward_velocity(vec3 forward_velocity) {
    camera.forward_velocity = forward_velocity;
}

void rotate_camera_yaw(float angle) {
    camera.yaw += angle; //incrementing by that angle
}

void rotate_camera_pitch(float angle) {
    camera.pitch += angle;
}

vec3 get_camera_lookat_target(void) {
    //Intialize the targe looking at the positive z-axis
    vec3 target = { 0, 0, 1 };

    mat4 camera_yaw_rotation = mat_rotate_y(camera.yaw);
    mat4 camera_pitch_rotation = mat_rotate_x(camera.pitch);

    //Create camera rotation matrix based on yaw, pitch, roll
    mat4 camera_rotation = mat_eye();
    camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

    //Update camera direction based on the rotation
    vec4 camera_direction = mat4_mul_vec4(camera_rotation, vec3_to_vec4(target));
    camera.direction = vec4_to_vec3(camera_direction);

    //Offset the camera position in the direction where the camera is pointing at
    target = vec3_add(camera.position, camera.direction);

    return target; 
}