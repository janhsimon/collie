#include "player.h"

#include "collision.h"
#include "geometry.h"
#include "input.h"
#include "level.h"
#include "shader.h"

#include <cglm/affine.h>
#include <cglm/vec3.h>

#include <glad/gl.h>

#include <stddef.h>
#include <stdint.h>

#define PLAYER_RADIUS 0.75f
#define PLAYER_HEIGHT 3.0f
#define PLAYER_MOVE_SPEED 10.0f

static const vec4 color_hit = { 1.0f, 0.0f, 0.0f, 1.0f };
static const vec4 color_miss = { 1.0f, 1.0f, 1.0f, 1.0f };

static vec3 gravity = { 0.0f, -0.1f, 0.0f };

static GLuint shader_program;
static GLint world_uniform_location, viewproj_uniform_location, color_uniform_location;

static struct Geometry* sphere = NULL;

static mat4 transform;
static vec3 velocity;
static bool in_contact;

bool generate_player()
{
  glm_translate_make(transform, (vec3){ 0.0f, 0.01f, 0.0f });

  sphere = make_geometry("objects/sphere.obj", GEOMETRY_TYPE_QUADS);
  if (!sphere)
  {
    return false;
  }

  // Generate shader program
  {
    GLuint vert, frag;
    if (!load_shader("shaders/player.vert.glsl", GL_VERTEX_SHADER, &vert))
    {
      destroy_geometry(sphere);
      return false;
    }

    if (!load_shader("shaders/player.frag.glsl", GL_FRAGMENT_SHADER, &frag))
    {
      destroy_geometry(sphere);
      return false;
    }

    if (!generate_shader_program(vert, frag, &shader_program))
    {
      destroy_geometry(sphere);
      return false;
    }

    glUseProgram(shader_program);

    // Retrieve uniform locations
    world_uniform_location = get_uniform_location(shader_program, "world");
    viewproj_uniform_location = get_uniform_location(shader_program, "viewproj");
    color_uniform_location = get_uniform_location(shader_program, "color");
  }

  return true;
}

void destroy_player()
{
  destroy_shader(shader_program);
  destroy_geometry(sphere);
}

mat4* get_player_transform()
{
  return &transform;
}

float get_player_height()
{
  return PLAYER_HEIGHT;
}

void update_player(const vec2 cursor_delta, float delta_time)
{
  // Yaw the player based on cursor movement
  glm_rotate(transform, -cursor_delta[0], GLM_YUP);

  // Move the player
  {
    glm_vec3_zero(velocity);

    // Keyboard input
    {
      if (is_key_down(KEY_W))
      {
        glm_vec3_add(velocity, transform[2], velocity);
      }

      if (is_key_down(KEY_S))
      {
        glm_vec3_sub(velocity, transform[2], velocity);
      }

      if (is_key_down(KEY_A))
      {
        glm_vec3_add(velocity, transform[0], velocity);
      }

      if (is_key_down(KEY_D))
      {
        glm_vec3_sub(velocity, transform[0], velocity);
      }
    }

    // Also add a bit of gravity
    glm_vec3_add(velocity, gravity, velocity);

    glm_vec3_scale_as(velocity, delta_time * PLAYER_MOVE_SPEED, velocity);
    glm_vec3_add(transform[3], velocity, transform[3]);
  }

  // Player-level collision
  {
    in_contact = false;

    // Capsule
    vec3 tip;
    glm_vec3_copy(transform[3], tip);
    tip[1] += PLAYER_HEIGHT;

    vec3 pen_normal;
    float pen_depth;
    for (uint32_t triangle = 0; triangle < get_triangle_count(); ++triangle)
    {
      vec3 v0, v1, v2, n;
      get_triangle(triangle, v0, v1, v2, n);

      if (capsule_triangle_collision(transform[3], tip, PLAYER_RADIUS, v0, v1, v2, n, pen_normal, &pen_depth))
      {
        in_contact = true;

        glm_vec3_scale(pen_normal, pen_depth, pen_normal);
        glm_vec3_add(transform[3], pen_normal, transform[3]);
      }
    }
  }
}

void draw_player(const mat4 viewproj_matrix)
{
  glBindVertexArray(sphere->vertex_array);
  glUseProgram(shader_program);

  // Set uniforms
  glUniformMatrix4fv(viewproj_uniform_location, 1, GL_FALSE, (float*)viewproj_matrix);
  glUniform4fv(color_uniform_location, 1, in_contact ? color_hit : color_miss);

  // Lower sphere
  mat4 sphere_matrix;
  {
    glm_mat4_copy(transform, sphere_matrix);
    glm_translate(sphere_matrix, (vec3){ 0.0f, PLAYER_RADIUS, 0.0f });
    glm_scale(sphere_matrix, (vec3){ PLAYER_RADIUS, PLAYER_RADIUS, PLAYER_RADIUS });
    glUniformMatrix4fv(world_uniform_location, 1, GL_FALSE, (float*)sphere_matrix);

    // Draw the sphere in individual quads
    for (uint32_t index = 0; index < sphere->index_count; index += 4)
    {
      glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(index * sizeof(uint32_t)));
    }
  }

  // Upper sphere
  {
    glm_mat4_copy(transform, sphere_matrix);
    glm_translate(sphere_matrix, (vec3){ 0.0f, PLAYER_HEIGHT - PLAYER_RADIUS, 0.0f });
    glm_scale(sphere_matrix, (vec3){ PLAYER_RADIUS, PLAYER_RADIUS, PLAYER_RADIUS });
    glUniformMatrix4fv(world_uniform_location, 1, GL_FALSE, (float*)sphere_matrix);

    // Draw the sphere in individual quads
    for (uint32_t index = 0; index < sphere->index_count; index += 4)
    {
      glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(index * sizeof(uint32_t)));
    }
  }
}
