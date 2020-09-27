#ifndef VOXEL_STATE_H
#define VOXEL_STATE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cengine/chunk.h>

#define CHUNK_RENDER_RADIUS 6

extern chunk_t *chunks;
extern uint16_t chunk_count;

void voxel_state_init();
void voxel_state_destroy();
void voxel_state_update(float deltaTime);
void voxel_state_draw();
void voxel_state_on_key(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
