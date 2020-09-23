#ifndef SHADER_H
#define SHADER_H

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

#endif
