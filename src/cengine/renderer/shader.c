#include "shader.h"

#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

unsigned int shader_create(const char* vertex_source, const char* fragment_source){
  int success;

  unsigned int vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_source, NULL);
  glCompileShader(vertex_shader);

  // check compile errors
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if(!success){
    char infoLog[512];
    glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
    printf("vertex shader error\n");
    printf(infoLog);
    printf("\n");

    glDeleteShader(vertex_shader);
    return 0;
  }

  unsigned int fragment_shader;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_source, NULL);
  glCompileShader(fragment_shader);

  // check compile errors
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if(!success){
    char infoLog[512];
    glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
    printf("fragment shader error\n");
    printf(infoLog);
    printf("\n");

    glDeleteShader(fragment_shader);
    return 0;
  }

  unsigned int shader_program;
  shader_program = glCreateProgram();

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // check compile errors
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success){
    char infoLog[512];
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("shader program error\n");
    printf(infoLog);
    printf("\n");

    glDeleteShader(fragment_shader);
    return 0;
  }

  return shader_program;
}

void shader_bind(unsigned int shader){
  glUseProgram(shader);
}

void shader_delete(unsigned int shader){
  glDeleteProgram(shader);
  shader = 0;
}

unsigned int shader_uniform_position(unsigned int shader, const char* name){
  return glGetUniformLocation(shader, name);
}

// uniform setters

void shader_uniform1i(unsigned int shader, const char* name, int value){
  glUniform1i(shader_uniform_position(shader, name), value);
}

void shader_uniform1f(unsigned int shader, const char* name, float value){
  glUniform1f(shader_uniform_position(shader, name), value);
}

void shader_uniform3fv(unsigned int shader, const char* name, float* value){
  glUniform3fv(shader_uniform_position(shader, name), 1, value);
}

void shader_uniform4fv(unsigned int shader, const char* name, float* value){
  glUniform4fv(shader_uniform_position(shader, name), 1, value);
}

void shader_uniform_matrix4fv(unsigned int shader, const char* name, float* value){
  shader_uniform_matrix4fv_at(shader_uniform_position(shader, name), value);
}

void shader_uniform_matrix4fv_at(unsigned int position, float* value){
  glUniformMatrix4fv(position, 1, GL_FALSE, value);
}
