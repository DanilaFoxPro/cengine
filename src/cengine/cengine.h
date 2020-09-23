#ifndef CENEGINE_H
#define CENEGINE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define CGLM_ALL_UNALIGNED
#include <cglm/cglm.h>
#include <cglm/struct.h>

#include "state_manager.h"

struct CEngineOptions{
  char *title;
  unsigned short width;
  unsigned short height;
  unsigned char antialiasing;
};

typedef struct{
  char quit;
  unsigned short width;
  unsigned short height;
  GLFWwindow* window;
  StateManager state_manager;
} CEngine;

int cengine_init(CEngine *cengine, struct CEngineOptions *options, GLFWkeyfun on_key);
void cengine_update(CEngine *cengine);
void cengine_free(CEngine *cengine);

#endif
