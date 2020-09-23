#ifndef DRAWABLE_H
#define DRAWABLE_H

#define CGLM_ALL_UNALIGNED
#include <cglm/cglm.h>

typedef struct{
  unsigned int vao;
  unsigned short vertex_count;
} drawable;

typedef struct{
  float position[3];
  float uv[2];
  float normal[3];
  float tangent[3];
  float bitangent[3];
} vertex;

drawable *drawable_create(/*struct model *model*/);
void drawable_free(drawable *drawable);
void drawable_draw(drawable *drawable);

#endif
