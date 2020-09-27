#define STB_IMAGE_IMPLEMENTATION	
#include <stb_image.h>

#include <cengine/cengine.h>

const char* RES_PATH = "res/";

unsigned int texture_create(const char* name, GLenum filter){
  mark_important_func();
  
  mark_important_stage("setup");
  unsigned int texture;
  glGenTextures(1, &texture);
  texture_bind(texture, 0);

  uint RES_PATH_SIZE = strlen(RES_PATH);
  uint name_length = strlen(name);
  
  mark_important_stage("concate res path and file name");
  // +1 because of terminating character.
  char* name_buffer = malloc(RES_PATH_SIZE + name_length + 1);
  strcpy(name_buffer, RES_PATH);
  strcpy(name_buffer+RES_PATH_SIZE, name);
  
  mark_important_stage("load image");
  // load image, create texture and generate mipmaps	
  int width, height, channels;
  stbi_set_flip_vertically_on_load(1);
  uint8_t* data = stbi_load(name_buffer, &width, &height, &channels, STBI_rgb_alpha);

  free(name_buffer);
  
  mark_important_stage("check data");
  if(data){
    mark_important_stage("transfer texture to OpenGL");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    // Only free data if it isn't a null pointer.
    stbi_image_free(data);
  }else{
    fprintf(stderr, "failed to load texture: %s\n", name_buffer);
  }
  
  mark_important_stage("free data");

  return texture;
}

unsigned int texture_create_(const unsigned char* raw_data){
  unsigned int texture;
  glGenTextures(1, &texture);
  texture_bind(texture, 0);

  int width, height, channels;
  // stbi_set_flip_vertically_on_load(1);
  printf("%d\n", sizeof(&raw_data[0]));
  unsigned char* data = stbi_load_from_memory(&raw_data[0], sizeof(&raw_data), &width, &height, &channels, STBI_rgb);

  if(data){
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }else{
    fprintf(stderr, "failed to load texture\n");
    return 0;
  }

  stbi_image_free(data);

  return texture;
}

void texture_bind(unsigned int texture, unsigned char index){
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, texture);
}

void texture_delete(unsigned int *texture){
  glDeleteTextures(1, texture);
  *texture = 0;
}
