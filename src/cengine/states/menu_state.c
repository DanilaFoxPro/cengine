#include "menu_state.h"

#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>
#define CGLM_ALL_UNALIGNED
#include <cglm/cglm.h>
#include <cglm/struct.h>

#include "../renderer/shader.h"
#include "../renderer/texture.h"


const float quad_vertices[] = {
   0.25f,  0.1f,
  -0.25f,  0.1f,
  -0.25f, -0.1f,
   0.25f, -0.1f
};

const unsigned char quad_indices[] = {
  0, 1, 2, 2, 3, 0
};

const char* vertex_shader_source = ""
  #include "../shaders/basic.vs"
;

const char* fragment_shader_source = ""
  #include "../shaders/basic.fs"
;

unsigned int menu_VAO, menu_VBO, menu_EBO, menu_shader;

void menu_state_init(){
  printf("menu state init\n");

  glGenVertexArrays(1, &menu_VAO);
  glGenBuffers(1, &menu_VBO);
  glGenBuffers(1, &menu_EBO);

  glBindVertexArray(menu_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, menu_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menu_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glDeleteBuffers(1, &menu_VBO);
  glDeleteBuffers(1, &menu_EBO);

  menu_shader = shader_create(vertex_shader_source, fragment_shader_source);
  shader_bind(menu_shader);

  mat4 model = GLMS_MAT4_IDENTITY_INIT;
  glm_translate(model, (vec3){0.0f, 0.0f, 0.0f});
  shader_uniform_matrix4fv(menu_shader, "model", model[0]);
}

void menu_state_destroy(){
  printf("menu state destroy\n");

  shader_delete(menu_shader);

  glDeleteVertexArrays(1, &menu_VAO);
  glDeleteBuffers(1, &menu_VBO);
  glDeleteBuffers(1, &menu_EBO);
}

void menu_state_update(float deltaTime){
}

void menu_state_draw(){
  shader_bind(menu_shader);

  glBindVertexArray(menu_VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}
