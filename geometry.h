#pragma once

#include <glad/gl.h>

#include <stdint.h>

struct Geometry
{
  float* vertices;
  uint32_t* indices;
  uint32_t index_count;
  uint32_t floats_per_vertex;
  GLuint vertex_array, vertex_buffer, index_buffer;
};

enum GeometryType
{
  GEOMETRY_TYPE_TRIS = 3,
  GEOMETRY_TYPE_QUADS = 4
};

struct Geometry* make_geometry(const char* filename, enum GeometryType type);
void destroy_geometry(struct Geometry* geometry);
