#include "skybox.h"

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "renderer/shader.h"
#include "camera.h"

unsigned int skybox_shader, skybox_projection_location, skybox_view_location;

#define DAY_COLOR (vec3){0.3f, 0.6f, 0.8f}
#define SUNSET_COLOR (vec3){0.8f, 0.4f, 0.1f}

const short skybox_vertices[] = {
   SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
  -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
  -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
   SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
  -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
   SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
   SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
  -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
  -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
  -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
  -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
  -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
   SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
   SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
   SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
   SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
   SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
  -SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
  -SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
   SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
  -SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
   SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
   SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
  -SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE
};

const unsigned char skybox_indices[] = {
  2, 1, 0, 0, 3, 2,
  6, 5, 4, 4, 7, 6,
  10, 9, 8, 8, 11, 10,
  14, 13, 12, 12, 15, 14,
  18, 17, 16, 16, 19, 18,
  22, 21, 20, 20, 23, 22
};

const char* skybox_vertex_shader_source = ""
  #include "shaders/skybox.vs"
;

const char* skybox_fragment_shader_source = ""
  #include "shaders/skybox.fs"
;

void skybox_init(){
  skybox_shader = shader_create(skybox_vertex_shader_source, skybox_fragment_shader_source);
  skybox_projection_location = shader_uniform_position(skybox_shader, "projection");
  skybox_view_location = shader_uniform_position(skybox_shader, "view");
}

void skybox_free(){
  shader_delete(skybox_shader);
}

void skybox_projection(float* projection_matrix){
  shader_bind(skybox_shader);
  shader_uniform_matrix4fv_at(skybox_projection_location, projection_matrix);
}

void skybox_create(Skybox *skybox){
  unsigned int vbo, ebo;
  glGenVertexArrays(1, &skybox->vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(skybox->vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indices), skybox_indices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_SHORT, GL_FALSE, 3 * sizeof(short), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

void skybox_delete(Skybox *skybox){
  glDeleteVertexArrays(1, &skybox->vao);
}

void skybox_draw(Skybox *skybox){
  shader_bind(skybox_shader);
  shader_uniform_matrix4fv_at(skybox_view_location, view[0]);

  vec3 color = GLMS_VEC3_ZERO_INIT;
  glm_vec3_lerp(SUNSET_COLOR, DAY_COLOR, sin(glfwGetTime() / 6.0f) / 2.0f + 0.5f, color);
  shader_uniform3fv(skybox_shader, "color", color);

  glBindVertexArray(skybox->vao);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
}
