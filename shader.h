#pragma once

#include <cglm/types.h>

#include <glad/gl.h>

#include <stdbool.h>

bool load_shader(const char* filename, GLenum type, GLuint* shader);
void destroy_shader(GLuint shader_program);

bool generate_shader_program(GLuint vertex_shader, GLuint fragment_shader, GLuint* shader_program);

GLint get_uniform_location(GLuint shader_program, const char* name);
void set_uniform_i(GLuint shader_program, const char* name, int value);
void set_uniform_vec4(GLuint shader_program, const char* name, const vec4 value);
