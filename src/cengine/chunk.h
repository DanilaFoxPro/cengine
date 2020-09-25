#ifndef CHUNK_H
#define CHUNK_H

#include <stdint.h>

#define GLEW_STATIC
#include "GL/glew.h"

#include <cengine/blocks.h>

#define CHUNK_SIZE 32
#define CHUNK_SIZE_SQUARED 1024
#define CHUNK_SIZE_CUBED 32768

typedef GLbyte byte4[4];
typedef GLbyte byte3[3];
typedef int vec2i[2];

typedef struct chunk{
  BlockIdentifier *blocks;
  unsigned int vao;
  int elements;
  uint8_t changed;
  uint8_t mesh_changed;
  int x;
  int y;
  int z;
  byte4 *vertex;
  char *brightness;
  byte3 *normal;
  float *texCoords;
  // neighboring chunks
  struct chunk *px;
  struct chunk *nx;
  struct chunk *py;
  struct chunk *ny;
  struct chunk *pz;
  struct chunk *nz;
} chunk_t;

unsigned int block_index(uint8_t x, uint8_t y, uint8_t z);
chunk_t chunk_init(int x, int y, int z);
void chunk_free(chunk_t *chunk);
unsigned char chunk_update(chunk_t *chunk);
void chunk_draw(chunk_t *chunk);
BlockIdentifier chunk_get(chunk_t *chunk, int x, int y, int z);
void chunk_set(chunk_t *chunk, int x, int y, int z, uint8_t block);

#endif
