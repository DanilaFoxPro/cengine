#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef void (*fnPtr)();
typedef void (*fnPtrFl)(float);

typedef struct{
  fnPtr init;
  fnPtr destroy;
  fnPtrFl update;
  fnPtr draw;
  GLFWkeyfun on_key;
} State;

typedef struct{
  State **stack;
  int capacity;
  int top;
} StateManager;

void state_manager_init(StateManager *state_manager);
void state_manager_free(StateManager *state_manager);
int state_manager_push(StateManager *state_manager, State *state);
int state_manager_pop(StateManager *state_manager);
State *state_manager_top(StateManager *state_manager);
void state_manager_update(StateManager *state_manager, float deltatime);
void state_manager_draw(StateManager *state_manager);

#endif
