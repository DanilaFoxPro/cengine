#ifndef CAMERA_H
#define CAMERA_H

#define CGLM_ALL_UNALIGNED
#include <cglm/cglm.h>
#include <cglm/struct.h>

struct camera_t {
  vec3 position;
  mat4 view;
  float yaw;
  float pitch;
  float dy;
  vec3 front;
};

extern struct camera_t camera;

void camera_init();
void camera_update();
void camera_update_position();
void camera_update_rotation();
void camera_move_forward(float amount);
void camera_move_right(float amount);

#endif
