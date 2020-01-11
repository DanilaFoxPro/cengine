#ifndef MODELS_H
#define MODELS_H

#define CGLM_ALL_UNALIGNED
#include <cglm\cglm.h>
#include <cglm\struct.h>

typedef enum{
  CUBE = 0,
  PLANE,
  SHERE,
  MONKEY,
  SPAWN_BEACON
} model_id;

struct model{
  float *vertices;
  unsigned int vertex_count;
} models[5];

#endif
