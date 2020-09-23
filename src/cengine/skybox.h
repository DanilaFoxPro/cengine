#ifndef SKYBOX_H
#define SKYBOX_H

#define SKYBOX_SIZE 1000

typedef struct{
  unsigned int vao;
} Skybox;

void skybox_init();
void skybox_free();
void skybox_projection(float* projection_matrix);
void skybox_create(Skybox *skybox);
void skybox_delete(Skybox *skybox);
void skybox_draw(Skybox *skybox, float time);

#endif
