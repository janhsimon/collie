#pragma once

#include <cglm/types.h>

#include <glad/gl.h>

#include <stdbool.h>
#include <stdint.h>

bool generate_level();
void destroy_level();
void draw_level(mat4 viewproj_matrix);

uint32_t get_triangle_count();
void get_triangle(uint32_t index, vec3 v0, vec3 v1, vec3 v2, vec3 n);
