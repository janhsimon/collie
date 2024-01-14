#include "camera.h"

#include "level.h"

#include <cglm/cam.h>
#include <cglm/ray.h>
#include <cglm/vec3.h>

#include <stdio.h>

#define NEAR 0.1f
#define FAR 1000.0f
#define FOV GLM_PI_4 // 45deg, vertical field of view in radians

#define PITCH_MIN -0.5f
#define PITCH_MAX (GLM_PI_2f - 0.001f)
#define DIST_MIN 1.0f
#define DIST_MAX 32.0f

#define WALL_DISTANCE 0.15f

static float cam_pitch = GLM_PI_4f;
static float cam_dist = 16.0f;

static mat4 proj_matrix;

void update_proj_matrix(float aspect)
{
  glm_perspective(FOV, aspect, NEAR, FAR, proj_matrix);
}

void update_camera(const vec2 cursor_delta,
                   float scroll_delta,
                   mat4 player_transform,
                   float player_height,
                   mat4 viewproj_matrix)
{
  // Pitch
  cam_pitch = glm_clamp(cam_pitch + cursor_delta[1], PITCH_MIN, PITCH_MAX);

  // Zoom
  cam_dist = glm_clamp(cam_dist - scroll_delta, DIST_MIN, DIST_MAX);

  // Calculate camera target and position
  vec3 head;
  vec3 cam_pos, cam_target;
  {
    glm_vec3_copy(player_transform[3], head);
    head[1] += player_height;

    vec3 look;
    glm_vec3_scale(player_transform[2], 100.0f, look);
    glm_vec3_rotate(look, cam_pitch, player_transform[0]);

    // Target
    glm_vec3_add(head, look, cam_target);

    // Position
    glm_vec3_normalize(look);
    glm_vec3_scale(look, cam_dist, look);
    glm_vec3_sub(head, look, cam_pos);
  }

  // Camera-level collision
  {
    vec3 dir;
    glm_vec3_sub(cam_pos, head, dir);

    float min_dist = -1.0f;
    for (uint32_t triangle = 0; triangle < get_triangle_count(); ++triangle)
    {
      vec3 v0, v1, v2, n;
      get_triangle(triangle, v0, v1, v2, n);

      float dist;
      bool hit = glm_ray_triangle(head, dir, v0, v1, v2, &dist);
      if (hit && dist < 1.0 + WALL_DISTANCE && (min_dist < 0.0f || dist < min_dist))
      {
        min_dist = dist;
      }
    }

    if (min_dist > 0.0f)
    {
      glm_vec3_negate(dir);
      glm_vec3_muladds(dir, 1.0f - min_dist + WALL_DISTANCE, cam_pos);
    }
  }

  // Calculate view projection matrix
  {
    glm_lookat(cam_pos, cam_target, GLM_YUP, viewproj_matrix); // View matrix
    glm_mat4_mul(proj_matrix, viewproj_matrix, viewproj_matrix);
  }
}
