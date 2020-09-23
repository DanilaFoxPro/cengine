#include "state_manager.h"

#include <stdio.h>
#include <stdlib.h>

void state_manager_init(StateManager *state_manager){
  state_manager->capacity = 3;
  state_manager->stack = malloc(state_manager->capacity * sizeof(State*));
  state_manager->top = -1;
}

void state_manager_free(StateManager *state_manager){
  do{
    state_manager_pop(state_manager);
  }while(state_manager->top > -1);

  free(state_manager->stack);
}

int state_manager_scale(StateManager *state_manager){
  state_manager->capacity *= 2;
  state_manager->stack = realloc(state_manager->stack, state_manager->capacity * sizeof(State*));
  return state_manager->capacity;
}

int state_manager_push(StateManager *state_manager, State *state){
  if(state_manager->top + 1 == state_manager->capacity){
    state_manager_scale(state_manager);
  }

  state_manager->top++;
  state_manager->stack[state_manager->top] = state;
  if(state->init != NULL){
    state->init();
  }

  return state_manager->top;
}

int state_manager_pop(StateManager *state_manager){
  if(state_manager->top == -1){
    return 0;
  }

  State *top = state_manager_top(state_manager);
  if(top->destroy != NULL){
    top->destroy();
  }

  state_manager->stack[state_manager->top] = NULL;
  state_manager->top--;
  return state_manager->top;
}

void state_manager_update(StateManager *state_manager, float deltatime){
  if(state_manager->top < 0){
    return;
  }
  State *state = state_manager_top(state_manager);
  if(state->update != NULL){
    state->update(deltatime);
  }
}

void state_manager_draw(StateManager *state_manager){
  if(state_manager->top < 0){
    return;
  }
  State *state = state_manager_top(state_manager);
  if(state->draw != NULL){
    state->draw();
  }
}

State *state_manager_top(StateManager *state_manager){
  return state_manager->stack[state_manager->top];
}
