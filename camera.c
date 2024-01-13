#include "camera.h"

#include "level.h"

#include <cglm/cam.h>
#include <cglm/ray.h>
#include <cglm/vec3.h>

#define NEAR 0.1f
#define FAR 1000.0f
#define FOV GLM_PI_4 // 45deg, vertical field of view in radians

#define PITCH_MIN -0.5f
#define PITCH_MAX (GLM_PI_2f - 0.001f)
#define DIST_MIN 2.0f
#define DIST_MAX 32.0f

#define WALL_DISTANCE 0.15f

static vec3 cam_pos, cam_target;
static float cam_pitch = GLM_PI_4f;
static float cam_dist = 16.0f;

static mat4 proj_matrix;

void update_proj_matrix(float aspect)
{
  glm_perspective(FOV, aspect, NEAR, FAR, proj_matrix);
}

void update_camera(const vec2 cursor_delta, float scroll_delta, mat4 player_transform, mat4 viewproj_matrix)
{
  // Pitch
  cam_pitch = glm_clamp(cam_pitch + cursor_delta[1], PITCH_MIN, PITCH_MAX);

  // Zoom
  cam_dist = glm_clamp(cam_dist - scroll_delta, DIST_MIN, DIST_MAX);

  // Calculate camera target and position
  {
    // Target
    glm_vec3_copy(player_transform[3], cam_target);
    cam_target[1] += 1.0f;

    // Position
    glm_vec3_copy(player_transform[3], cam_pos);
    glm_vec3_muladds(player_transform[1], sinf(cam_pitch) * cam_dist, cam_pos);
    glm_vec3_muladds(player_transform[2], -cosf(cam_pitch) * cam_dist, cam_pos);
  }

  // Camera-level collision
  {
    vec3 dir;
    glm_vec3_sub(cam_pos, cam_target, dir);

    float min_dist = -1.0f;
    for (uint32_t triangle = 0; triangle < get_triangle_count(); ++triangle)
    {
      vec3 v0, v1, v2, n;
      get_triangle(triangle, v0, v1, v2, n);

      float dist;
      bool hit = glm_ray_triangle(player_transform[3], dir, v0, v1, v2, &dist);
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
