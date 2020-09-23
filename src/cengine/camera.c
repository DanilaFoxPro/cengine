#include "camera.h"

#include <math.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "main.h"

#include <string.h> // memcpy

struct camera_t camera;

/** Set default camera position, rotation, and view. */
void camera_init(){
  
  const mat4 view = GLMS_MAT4_IDENTITY_INIT;
  memcpy( &camera.view, &view, sizeof(mat4) );
  
  camera.position[0] = 0.0f;
  camera.position[1] = 32.0f;
  camera.position[2] = 0.0f;
  
  camera.yaw = -90.0f;
  camera.pitch = 0.0f;
  camera.dy = 0.0f;
  camera.front[0] = 0.0f;
  camera.front[1] = 0.0f;
  camera.front[2] = -1.0f;
  
  camera_update_rotation();
  camera_update_position();
}

void camera_update(){
#if WALKING
  if(glfwGetKey(cengine.window, GLFW_KEY_SPACE) && camera.dy == 0.0f){
    camera.dy = 0.1f;
  }

  camera.dy -= deltaTime * 1.0f;
  camera.dy = MAX(camera.dy, -5.0f);
  camera.position[1] += camera.dy;
#endif

  camera_update_rotation();
  camera_update_position();
}

void camera_update_position(){
  vec3 center = GLMS_VEC3_ZERO_INIT;
  glm_vec3_add(camera.position, camera.front, center);
  glm_lookat(camera.position, center, (vec3){0.0f, 1.0f, 0.0f}, camera.view);
}

void camera_update_rotation(){
  vec3 front = GLMS_VEC3_ZERO_INIT;
  front[0] = cos(glm_rad(camera.yaw )) * cos(glm_rad(camera.pitch ));
  front[1] = sin(glm_rad(camera.pitch ));
  front[2] = sin(glm_rad(camera.yaw )) * cos(glm_rad(camera.pitch ));
  glm_normalize(front);
  glm_vec3_copy(front, camera.front );
}

void camera_move_forward(float amount){
#if WALKING
  glm_vec3_add(camera.camera_position, (vec3){camera_front[0] * amount, 0.0f, camera_front[2] * amount}, camera_position);
#else
  glm_vec3_add(camera.position, (vec3){camera.front[0] * amount, camera.front[1] * amount, camera.front[2] * amount}, camera.position );
#endif
}

void camera_move_right(float amount){
  vec3 a = GLMS_VEC3_ZERO_INIT;
  glm_cross(camera.front, (vec3){0.0f, 1.0f, 0.0f}, a);
  glm_normalize(a);
  glm_vec3_add(camera.position, (vec3){a[0] * amount, a[1] * amount, a[2] * amount}, camera.position );
}
