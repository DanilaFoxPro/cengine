#include "voxel_state.h"

// #define MULTI_THREADING

#include <stdio.h>
#include <string.h>
#ifdef MULTI_THREADING
#include <pthread.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define CGLM_ALL_UNALIGNED
#include <cglm/cglm.h>
#include <cglm/struct.h>

#include <cengine/renderer/shader.h>
#include <cengine/renderer/texture.h>
#include <cengine/main.h>
#include <cengine/camera.h>
#include <cengine/skybox.h>
#include <cengine/blocks.h>
#include <cengine/chunk.h>

#include <cengine/debug/recovery.h>

#define MAX_CHUNKS_GENERATED_PER_FRAME 8
#define REACH_DISTANCE 12.0f

const static int8_t FACES[6][3] = {
  { 1, 0, 0},
  {-1, 0, 0},
  { 0, 1, 0},
  { 0,-1, 0},
  { 0, 0, 1},
  { 0, 0,-1}
};

const static char* voxel_vertex_shader_source = ""
  #include "../shaders/voxel.vs"
;

const static char* voxel_fragment_shader_source = ""
  #include "../shaders/voxel.fs"
;

chunk_t *chunks;
unsigned short chunk_count = 0;
unsigned short chunks_capacity = 4096;

unsigned int shader, texture, projection_location, view_location, camera_position_location, light_direction_location, daylight_location;
Skybox skybox;
mat4 projection = GLMS_MAT4_IDENTITY_INIT;

#ifdef MULTI_THREADING
pthread_t chunk_update_thread_id;
unsigned char running = 1;
#endif

unsigned char reloadPress = 0;

uint8_t current_block = 1;
unsigned short day_length = 300;

float dti(float val){
  return fabsf(val - roundf(val));
}

float time_of_day(){
  if(day_length <= 0){
    return 0.5f;
  }

  float t = glfwGetTime() / day_length;
  t = t - (int)t;
  return t;
}

float get_daylight(){
  float timer = time_of_day();
  if(timer < 0.5f){
    float t = (timer - 0.25f) * 100.0f;
    return 1.0f / (1.0f + powf(2.0f, -t));
  }else{
    float t = (timer - 0.85f) * 100.0f;
    return 1.0f - 1.0f / (1.0f + powf(2.0f, -t));
  }
}

void get_sight_vector(float rx, float ry, float *vx, float *vy, float *vz){
  float m = cosf(ry);
  *vx = cosf(rx - glm_rad(90.0f)) * m;
  *vy = sinf(ry);
  *vz = sinf(rx - glm_rad(90.0f)) * m;
}

uint8_t _hit_test(uint8_t *blocks, float max_distance, float x, float y, float z, float vx, float vy, float vz, int *hx, int *hy, int *hz){
  uint8_t m = 32;
  int px = 0;
  int py = 0;
  int pz = 0;

  for(unsigned short i = 0; i < max_distance * m; i++){
    int nx = roundf(x);
    int ny = roundf(y);
    int nz = roundf(z);

    if(nx != px || ny != py || nz != pz){
      uint8_t block = blocks[block_index(nx % CHUNK_SIZE, ny % CHUNK_SIZE, nz % CHUNK_SIZE)];
      if(block > 0){
        printf("hit block %d at %d %d %d\n", block, nx % CHUNK_SIZE, ny % CHUNK_SIZE, nz % CHUNK_SIZE);
        *hx = nx;
        *hy = ny;
        *hz = nz;

        return 1;
      }

      px = nx;
      py = ny;
      pz = pz;
    }

    x += vx / m;
    y += vy / m;
    z += vz / m;
  }

  return 0;
}

uint8_t hit_test(float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz){
  uint8_t result = 0;
  float best = 0.0f;

  int cx = round(x) / CHUNK_SIZE;
  int cz = round(z) / CHUNK_SIZE;

  float vx, vy, vz;
  get_sight_vector(rx, ry, &vx, &vy, &vz);

  for(unsigned short i = 0; i < chunk_count; i++){
    chunk_t *chunk = &chunks[i];
    int dx = chunk->x - cx;
    int dz = chunk->z - cz;
    if(abs(dx) > 1 || abs(dz) > 1){
      continue;
    }

    int hx, hy, hz;
    if(_hit_test(chunk->blocks, 12.0f, x, y, z, vx, vy, vz, &hx, &hy, &hz)){
      float d = sqrtf(powf(hx - x, 2.0f) + powf(hy - y, 2.0f) + powf(hz - z, 2.0f));
      if(best == 0 || d < best){
        best = d;
        *bx = hx;
        *by = hy;
        *bz = hz;
      }

      result = 1;
    }
  }

  return result;
}

void update_chunks(){
  unsigned short chunk_meshes_generated = 0;

  for(unsigned short i = 0; i < chunk_count; i++){
    if(chunk_meshes_generated >= MAX_CHUNKS_GENERATED_PER_FRAME){
      // printf("generated max number of chunk meshes for frame\n");
      break;
    }

    chunk_meshes_generated += chunk_update(&chunks[i]);
  }
}

#ifdef MULTI_THREADING
void* chunk_update_thread(){
  while(running){
    update_chunks();

#ifdef _WIN32
    Sleep(0);
#else
    sleep(0);
#endif
  }

  return NULL;
}
#endif

uint8_t verc_ray_march(float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz, int *cx, int *cy, int *cz){
  int px = roundf(x) / CHUNK_SIZE;
  int py = roundf(y) / CHUNK_SIZE;
  int pz = roundf(z) / CHUNK_SIZE;

  float vx, vy, vz;
  get_sight_vector(rx, ry, &vx, &vy, &vz);

  float t = 0.0f;

  while(t < REACH_DISTANCE){
    float rayx = x + vx * t;
    float rayy = y + vy * t;
    float rayz = z + vz * t;

    for(unsigned short i = 0; i < chunk_count; i++){
      chunk_t *chunk = &chunks[i];
      int dx = chunk->x - px;
      int dy = chunk->y - py;
      int dz = chunk->z - pz;
      if(abs(dx) > 1 || abs(dy) > 1 || abs(dz) > 1){
        continue;
      }
      // if(chunk->x != cx || chunk->z != cz){
      //   continue;
      // }

      int nx = abs(roundf(rayx));
      int ny = abs(roundf(rayy));
      int nz = abs(roundf(rayz));

      uint8_t block = chunk->blocks[block_index(nx % CHUNK_SIZE, ny % CHUNK_SIZE, nz % CHUNK_SIZE)];
      if(block > 0){
        printf("chunk: %d %d\n", chunk->x, chunk->z);
        printf("hit block %d at %d %d %d\n", block, nx % CHUNK_SIZE, ny % CHUNK_SIZE, nz % CHUNK_SIZE);
        *bx = nx;
        *by = ny;
        *bz = nz;
        *cx = chunk->x;
        *cy = chunk->y;
        *cz = chunk->z;

        return 1;
      }
    }

    t += 0.5f;
  }

  return 0;
}

/** Tries to find chunk by chunk coordinates. Returns NULL on failure. */
chunk_t* chunk_find_by_coordinates( const int x, const int y, const int z )
{
        for(unsigned short i = 0; i < chunk_count; i++){
                chunk_t *chunk = &chunks[i];
                if( chunk->x == x
                 && chunk->y == y
                 && chunk->z == z){
                        return chunk;
                }
        }
        return NULL;
}

void ensure_chunks(int x, int y, int z){
  mark_important_func();

  mark_important_stage( "iterate over existing chunks" );
  for(unsigned short i = 0; i < chunk_count; i++){
    chunk_t *chunk = &chunks[i];
    int dx = x - chunk->x;
    int dy = y - chunk->y;
    int dz = z - chunk->z;

    // remove chunks outside of render radius
    if(abs(dx) > CHUNK_RENDER_RADIUS || abs(dy) > CHUNK_RENDER_RADIUS || abs(dz) > CHUNK_RENDER_RADIUS){
      chunk_free(chunk);

      chunk_t *other = chunks + (--chunk_count);
      memcpy(chunk, other, sizeof(chunk_t));
      
    }
  }

  mark_important_stage( "generate new chunks" );
  // generate new chunks needed
  unsigned short chunks_generated = 0;
  for(int8_t i = -CHUNK_RENDER_RADIUS; i <= CHUNK_RENDER_RADIUS; i++){
    for(int8_t j = -CHUNK_RENDER_RADIUS; j <= CHUNK_RENDER_RADIUS; j++){
      for(int8_t k = -CHUNK_RENDER_RADIUS; k <= CHUNK_RENDER_RADIUS; k++){
        int cx = x + i;
        int cy = y + k;
        int cz = z + j;

        // see if chunk already exists
        if( chunk_find_by_coordinates( cx, cy, cz ) != NULL ) {
          continue;
        }

        // expand chunk capacity if needed
        if( chunk_count + 1 >= chunks_capacity ){
                chunks_capacity *= 2;
                printf("reached max number of chunks, resizing to %d\n", chunks_capacity);
                
                chunk_t* old_chunks = chunks;
                chunks = realloc(chunks, chunks_capacity * sizeof(chunk_t));
                
                // 'realloc' returns a NULL pointer if it failed to re-allocate storage.
                // The original pointer should still be valid in that scenario.
                if( chunks == NULL ) {
                        printf( "Failed to reallocate chunk storage." );
                        chunks = old_chunks;
                        return;
                }
        }

        chunk_t chunk = chunk_init(cx, cy, cz);
        chunks[chunk_count] = chunk;
        chunk_count++;
        chunks_generated++;

        if(chunks_generated >= MAX_CHUNKS_GENERATED_PER_FRAME){
          // printf("generated max number of chunks for this frame\n");
          return;
        }
      }
    }
  }

  // printf("generated %d chunks\n", chunks_generated);
}

#if WALKING
uint8_t get_block(int x, int y, int z){
  vec2i chunk_position = {floor(x / CHUNK_SIZE), floor(z / CHUNK_SIZE)};
  vec2i block_local_position = {
    (CHUNK_SIZE + (x % CHUNK_SIZE)) % CHUNK_SIZE,
    (CHUNK_SIZE + (x % CHUNK_SIZE)) % CHUNK_SIZE
  };

  unsigned short access = block_index(block_local_position[0], y, block_local_position[1]);

  for(unsigned short i = 0; i < chunk_count; i++){
    chunk_t *other = chunks[i];
    if(other->x == chunk_position[0] && other->z == chunk_position[1]){
      return other->blocks[access];
    }
  }
}

uint8_t _collide(uint8_t height, float *_x, float *_y, float *_z){
  uint8_t result = 0;
  float pad = 0.25f;
  float x = *_x;
  float y = *_y;
  float z = *_z;
  int nx = round(x);
  int ny = round(y);
  int nz = round(z);
  float p[3] = {x, y, z};
  int np[3] = {nx, ny, nz};

  for(uint8_t face = 0; face < 6; face++){
    for(uint8_t i = 0; i < 3; i++){
      int8_t dir = FACES[face][i];
      if(!dir){
        continue;
      }

      float dist = (p[i] - np[i]) * dir;
      if(dist < pad){
        continue;
      }

      for(uint8_t dy = 0; dy < height; dy++){
        int op[3] = {nx, ny - dy, nz};
        op[i] += dir;

        if(!get_block(op[0], op[1], op[2])){
          continue;
        }

        p[i] -= (dist - pad) * dir;

        if(i == 1){
          result = 1;
        }

        break;
      }
    }
  }

  *_x = p[0];
  *_y = p[1];
  *_z = p[2];
  return result;
}

uint8_t collide(float *x, float *y, float *z){
  int px = round(*x) / CHUNK_SIZE;
  int pz = round(*z) / CHUNK_SIZE;

  for(unsigned short i = 0; i < chunk_count; i++){
    chunk_t *chunk = chunks[i];
    int dx = chunk->x - px;
    int dz = chunk->z - pz;

    if(abs(dx) > 1 || abs(dz) > 1){
      continue;
    }

    if(_collide(2, x, y, z)){
      return 1;
    }
  }

  return 0;
}
#endif

void voxel_state_init(){
  mark_important_func();
  printf("voxel state init\n");
  
  blocks_fill();
  
  chunks = malloc(chunks_capacity * sizeof(chunk_t));

  shader = shader_create(voxel_vertex_shader_source, voxel_fragment_shader_source);
  shader_bind(shader);

  projection_location = shader_uniform_position(shader, "projection");
  view_location = shader_uniform_position(shader, "view");
  camera_position_location = shader_uniform_position(shader, "camera_position");
  light_direction_location = shader_uniform_position(shader, "light_direction");
  daylight_location = shader_uniform_position(shader, "daylight");
  shader_uniform1i(shader, "diffuse_texture", 0);

  texture = texture_create("tiles.png", GL_NEAREST);

  skybox_init();
  skybox_create(&skybox);

  glfwSetTime(day_length / 3.0f);

#ifdef MULTI_THREADING
  if(pthread_create(&chunk_update_thread_id, NULL, &chunk_update_thread, NULL)){
    fprintf(stderr, "failed to create chunk mesh thread\n");
  }
#endif

  glfwSetInputMode(cengine.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void voxel_state_destroy(){
  printf("voxel state destroy\n");

  for(unsigned short i = 0; i < chunk_count; i++){
    chunk_free(&chunks[i]);
  }

  free(chunks);

  skybox_delete(&skybox);
  skybox_free();

  texture_delete(&texture);
  shader_delete(shader);

#ifdef MULTI_THREADING
  running = 0;
  pthread_join(chunk_update_thread_id, NULL);
#endif

  glfwSetInputMode(cengine.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void voxel_state_update(float deltaTime){
  mark_important_func();
  if(glfwGetKey(cengine.window, GLFW_KEY_F11) == GLFW_PRESS){
    reloadPress = 1;
  }else if(reloadPress == 1 && glfwGetKey(cengine.window, GLFW_KEY_F11) == GLFW_RELEASE){
    reloadPress = 0;
    printf("%.6f %.6f\n", camera.position[0], camera.position[2]);
    // printf("reloading...\n");
    // voxel_state_destroy();
    // voxel_state_init();
    // printf("done\n");
  }

  camera_update();
#if WALKING
  if(collide(&camera_position[0], &camera_position[1], &camera_position[2])){
    camera_dy = 0.0f;
  }
#endif

#ifndef MULTI_THREADING
  update_chunks();
#endif

  unsigned char left_mouse = glfwGetMouseButton(cengine.window, GLFW_MOUSE_BUTTON_LEFT);
  unsigned char right_mouse = glfwGetMouseButton(cengine.window, GLFW_MOUSE_BUTTON_RIGHT);
  
  if(left_mouse || right_mouse){
    int hx, hy, hz, cx, cy, cz;
    if(verc_ray_march(camera.position[0], camera.position[1], camera.position[2], camera.pitch, camera.yaw, &hx, &hy, &hz, &cx, &cy, &cz)){
      // int cx = hx / CHUNK_SIZE;
      // int cz = hz / CHUNK_SIZE;

      for(unsigned short i = 0; i < chunk_count; i++){
        chunk_t *chunk = &chunks[i];
        if(chunk->x == cx && chunk->z == cz){
          chunk_set(chunk, hx % CHUNK_SIZE, hy % CHUNK_SIZE, hz % CHUNK_SIZE, left_mouse ? 0 : current_block);
          break;
        }
      }
    }
  }
}

void voxel_state_draw(){
  mark_important_func();
  shader_bind(shader);
  texture_bind(texture, 0);

  const float daylight = get_daylight();
  glUniform1f(daylight_location, daylight);
  // vec3 light_direction = {sinf(glm_rad(daylight)), cosf(glm_rad(daylight)), 0.1f};
  // glUniform3fv(light_direction_location, 1, light_direction);

  glm_perspective(glm_rad(65.0f), (float)cengine.width/(float)cengine.height, 0.1f, SKYBOX_SIZE * 2.0f, projection);
  shader_uniform_matrix4fv_at(projection_location, projection[0]);

  glUniformMatrix4fv(view_location, 1, GL_FALSE, camera.view[0]);
  glUniform3fv(camera_position_location, 1, camera.position );

  int x = floorf(camera.position[0] / CHUNK_SIZE);
  int y = floorf(camera.position[1] / CHUNK_SIZE);
  int z = floorf(camera.position[2] / CHUNK_SIZE);
  ensure_chunks(x, y, z);

  unsigned short rendered_chunks = 0;
  for(unsigned short i = 0; i < chunk_count; i++){
    chunk_t *chunk = &chunks[i];

    // don't render chunk if empty or outside render radius
    if(!chunk->elements || abs(x - chunk->x) > CHUNK_RENDER_RADIUS || abs(y - chunk->y) > CHUNK_RENDER_RADIUS || abs(z - chunk->z) > CHUNK_RENDER_RADIUS){
      continue;
    }

    mat4 model = GLMS_MAT4_IDENTITY_INIT;
    glm_translate(model, (vec3){chunk->x * CHUNK_SIZE, chunk->y * CHUNK_SIZE, chunk->z * CHUNK_SIZE});
    shader_uniform_matrix4fv(shader, "model", model[0]);

    chunk_draw(chunk);
    rendered_chunks++;
  }
  // printf("rendered %d chunks\n", rendered_chunks);

  skybox_projection(projection[0]);
  skybox_draw(&skybox, daylight);
}

void voxel_state_on_key(GLFWwindow* window, int key, int scancode, int action, int mods){
  int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
  if(control && key == GLFW_KEY_V){
    const char* buffer = glfwGetClipboardString(window);

    if(strstr(buffer, "/reload") == buffer){
      texture_delete(&texture);
      texture = texture_create("tiles.png", GL_NEAREST);
    }
  }

  if(key >= '1' && key <= '6'){
    current_block = key - '1' + 1;
  }

  if(key == GLFW_KEY_E){
    current_block = current_block % 6 + 1;
  }
}
