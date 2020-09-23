#ifndef TEXTURE_H
#define TEXTURE_H

#define GLEW_STATIC
#include <GL/glew.h>

unsigned int texture_create(const char* name, GLenum filter);
void texture_bind(unsigned int texture, unsigned char index);
void texture_delete(unsigned int *texture);

#endif
