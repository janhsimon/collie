#include "level.h"

#include "geometry.h"
#include "shader.h"
#include "texture.h"

#include <stddef.h>

static GLuint shader_program;
static GLint viewproj_uniform_location;

static struct Geometry* geometry;
static GLuint texture;

bool generate_level()
{
  geometry = make_geometry("levels/level.obj", GEOMETRY_TYPE_TRIS);
  if (!geometry)
  {
    return false;
  }

  // Load texture
  if (!load_texture("textures/brick.png", &texture))
  {
    destroy_geometry(geometry);
    return false;
  }

  // Generate shader program
  {
    GLuint vert, frag;
    if (!load_shader("shaders/level.vert.glsl", GL_VERTEX_SHADER, &vert))
    {
      destroy_geometry(geometry);
      return false;
    }

    if (!load_shader("shaders/level.frag.glsl", GL_FRAGMENT_SHADER, &frag))
    {
      destroy_geometry(geometry);
      return false;
    }

    if (!generate_shader_program(vert, frag, &shader_program))
    {
      destroy_geometry(geometry);
      return false;
    }

    glUseProgram(shader_program);

    // Retrieve uniform locations
    viewproj_uniform_location = get_uniform_location(shader_program, "viewproj");

    // Set uniforms
    set_uniform_i(shader_program, "tex", 0);
  }

  return true;
}

void destroy_level()
{
  destroy_shader(shader_program);
  destroy_texture(texture);
  destroy_geometry(geometry);
}

void draw_level(mat4 viewproj_matrix)
{
  glBindVertexArray(geometry->vertex_array);
  glUseProgram(shader_program);

  // Set texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set view-projection matrix uniform
  glUniformMatrix4fv(viewproj_uniform_location, 1, GL_FALSE, (float*)viewproj_matrix);

  // Draw the level
  glDrawElements(GL_TRIANGLES, geometry->index_count, GL_UNSIGNED_INT, 0);
}

uint32_t get_triangle_count()
{
  return geometry->index_count / 3;
}

void get_triangle(uint32_t index, vec3 v0, vec3 v1, vec3 v2, vec3 n)
{
  v0[0] = geometry->vertices[geometry->indices[index * 3 + 0] * geometry->floats_per_vertex + 0];
  v0[1] = geometry->vertices[geometry->indices[index * 3 + 0] * geometry->floats_per_vertex + 1];
  v0[2] = geometry->vertices[geometry->indices[index * 3 + 0] * geometry->floats_per_vertex + 2];

  v1[0] = geometry->vertices[geometry->indices[index * 3 + 1] * geometry->floats_per_vertex + 0];
  v1[1] = geometry->vertices[geometry->indices[index * 3 + 1] * geometry->floats_per_vertex + 1];
  v1[2] = geometry->vertices[geometry->indices[index * 3 + 1] * geometry->floats_per_vertex + 2];

  v2[0] = geometry->vertices[geometry->indices[index * 3 + 2] * geometry->floats_per_vertex + 0];
  v2[1] = geometry->vertices[geometry->indices[index * 3 + 2] * geometry->floats_per_vertex + 1];
  v2[2] = geometry->vertices[geometry->indices[index * 3 + 2] * geometry->floats_per_vertex + 2];

  n[0] = geometry->vertices[geometry->indices[index * 3 + 0] * geometry->floats_per_vertex + 3];
  n[1] = geometry->vertices[geometry->indices[index * 3 + 0] * geometry->floats_per_vertex + 4];
  n[2] = geometry->vertices[geometry->indices[index * 3 + 0] * geometry->floats_per_vertex + 5];
}
