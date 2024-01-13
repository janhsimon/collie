#pragma once

#include <glad/gl.h>

#include <stdbool.h>

bool load_texture(const char* filename, GLuint* texture);
void destroy_texture(GLuint texture);
