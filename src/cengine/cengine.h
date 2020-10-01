#ifndef CENEGINE_H
#define CENEGINE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define CGLM_ALL_UNALIGNED
#include <cglm/cglm.h>
#include <cglm/struct.h>

#define F_MEMORY_DEBUG
#include <Forge/forge.h>

#define DEBUG
#include <cengine/debug/recovery.h>

#include <noise.h>

//:: State manager.

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

State state_get_empty();

void state_manager_init(StateManager *state_manager);
void state_manager_free(StateManager *state_manager);
int state_manager_push(StateManager *state_manager, State *state);
int state_manager_pop(StateManager *state_manager);
State *state_manager_top(StateManager *state_manager);
void state_manager_update(StateManager *state_manager, float deltatime);
void state_manager_draw(StateManager *state_manager);

//:: CEngine general.

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

extern CEngine cengine;

//:: Game state.

void game_state_init();
void game_state_destroy();
void game_state_update(float deltaTime);
void game_state_draw();

//:: Menu state.

void menu_state_init();
void menu_state_destroy();
void menu_state_update(float deltaTime);
void menu_state_draw();

//:: Camera stuff.

struct camera_t {
  vec3 position;
  mat4 view;
  float yaw;
  float pitch;
  float dy;
  vec3 front;
};

extern struct camera_t camera;

void camera_init();
void camera_update();
void camera_update_position();
void camera_update_rotation();
void camera_move_forward(float amount);
void camera_move_right(float amount);

//:: Block.

#include <stdint.h> // uint8_t

typedef uint8_t BlockIdentifier;
typedef int BlockSide;

struct BlockSides {
        BlockSide left;
        BlockSide right;
        BlockSide top;
        BlockSide bottom;
        BlockSide front;
        BlockSide back;
};

typedef uint8_t BlockInfoFlags;

enum {
        blockflag_opaque = 1,
};

struct BlockInfo {
        struct BlockSides sides;
        BlockInfoFlags flags;
};

enum block_tile {
        tile_dirt,
        tile_stone,
        tile_bedrock,
        tile_sand,
        tile_grass_side,
        tile_glass,
        tile_grass_top = 8,
};

enum block_index {
        blockid_air,
        blockid_grass,
        blockid_stone,
        blockid_dirt,
        blockid_bedrock,
        blockid_sand,
        blockid_glass,
        blockid_LIMIT
};

uint8_t block_is_transparent( const BlockIdentifier block_identifier );

const struct BlockSides blocksides_filled( BlockSide fill );
const struct BlockSides blocksides_common_fill( BlockSide sides, BlockSide top, BlockSide bottom );

void    blockflag_enable        ( BlockInfoFlags* flags, const BlockInfoFlags flag );
void    blockflag_disable       ( BlockInfoFlags* flags, const BlockInfoFlags flag );
void    blockflag_toggle        ( BlockInfoFlags* flags, const BlockInfoFlags flag );
uint8_t blockflag_get_value( const BlockInfoFlags flags, const BlockInfoFlags flag );

void blocks_fill();

extern struct BlockInfo blocks[256];

//:: Chunk.

#define CHUNK_SIZE 32
#define CHUNK_SIZE_SQUARED 1024
#define CHUNK_SIZE_CUBED 32768

typedef GLbyte byte4[4];
typedef GLbyte byte3[3];
typedef int vec2i[2];

typedef struct chunk{
  BlockIdentifier* blocks;
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

uint16_t block_index(uint8_t x, uint8_t y, uint8_t z);
chunk_t chunk_init(int x, int y, int z);
void chunk_free(chunk_t *chunk);
unsigned char chunk_update(chunk_t *chunk);
void chunk_draw(chunk_t *chunk);
BlockIdentifier chunk_get(chunk_t *chunk, int x, int y, int z);
void chunk_set(chunk_t *chunk, int x, int y, int z, uint8_t block);

//:: Voxel state.

#define CHUNK_RENDER_RADIUS 6

extern chunk_t *chunks;
extern uint16_t chunk_count;

void voxel_state_init();
void voxel_state_destroy();
void voxel_state_update(float deltaTime);
void voxel_state_draw();
void voxel_state_on_key(GLFWwindow* window, int key, int scancode, int action, int mods);

//:: Skybox.

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

//:: Texture.

unsigned int texture_create(const char* name, GLenum filter);
void texture_bind(unsigned int texture, unsigned char index);
void texture_delete(unsigned int *texture);

//:: Shader.

unsigned int shader_create(const char* vertex_source, const char* fragment_source);
void shader_bind(unsigned int shader);
void shader_delete(unsigned int shader);
unsigned int shader_uniform_position(unsigned int shader, const char* name);
// uniform setters
void shader_uniform1i(unsigned int shader, const char* name, int value);
void shader_uniform1f(unsigned int shader, const char* name, float value);
void shader_uniform3fv(unsigned int shader, const char* name, float* value);
void shader_uniform4fv(unsigned int shader, const char* name, float* value);
void shader_uniform_matrix4fv(unsigned int shader, const char* name, float* value);
void shader_uniform_matrix4fv_at(unsigned int position, float* value);

//:: Drawable.

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

//:: Render utility.

unsigned int make_buffer(GLenum target, GLsizei size, const void* data);

//:: Utility.

void clampf( float* Value, const float Min, const float Max );
void clampui( unsigned int* Value, const unsigned int Min, const unsigned int Max );
void check_memory();

#endif
