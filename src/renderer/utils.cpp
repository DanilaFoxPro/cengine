#include "utils.h"

unsigned int make_buffer(GLenum target, GLsizei size, const void* data){
  unsigned int buffer;

  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, GL_STATIC_DRAW);

  return buffer;
}