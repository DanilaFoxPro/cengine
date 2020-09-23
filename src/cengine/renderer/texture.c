#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION	
#include <stb_image.h>

#define RES_PATH "res/"

unsigned int texture_create(const char* name, GLenum filter){
  unsigned int texture;
  glGenTextures(1, &texture);
  texture_bind(texture, 0);

  unsigned char name_length = 0;
  for(const char* i = name; *i; i++){
    name_length++;
  }

  char *name_buffer = (char*)malloc(sizeof(RES_PATH) - 1 + name_length);
  strncpy(name_buffer, RES_PATH, sizeof(RES_PATH));	
  strncat(name_buffer, name, name_length);

  // load image, create texture and generate mipmaps	
  int width, height, channels;	
  stbi_set_flip_vertically_on_load(1);
  unsigned char* data = stbi_load(name_buffer, &width, &height, &channels, STBI_rgb_alpha);

  if(data){
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  }else{
    fprintf(stderr, "failed to load texture: %s\n", name_buffer);
  }

  free(name_buffer);
  stbi_image_free(data);

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
