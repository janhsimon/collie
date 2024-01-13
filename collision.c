#include "collision.h"

#include <cglm/util.h>
#include <cglm/vec3.h>

static void closest_point_on_line(vec3 a, vec3 b, vec3 p, vec3 cp)
{
  vec3 v;
  glm_vec3_sub(b, a, v);

  vec3 w;
  glm_vec3_sub(p, a, w);

  const float d0 = glm_vec3_dot(w, v);
  const float d1 = glm_vec3_dot(v, v);
  const float t = glm_min(glm_max(d0 / d1, 0.0f), 1.0f);
  glm_vec3_scale(v, t, v);

  glm_vec3_add(a, v, cp);
}

bool sphere_triangle_collision(vec3 sphere_pos,
                               float sphere_radius,
                               vec3 triangle_p0,
                               vec3 triangle_p1,
                               vec3 triangle_p2,
                               vec3 triangle_normal,
                               vec3 pen_normal,
                               float* pen_depth)
{
  // Step 1: Plane-sphere check
  float dist;
  {
    vec3 v;
    glm_vec3_sub(sphere_pos, triangle_p0, v);
    dist = glm_vec3_dot(v, triangle_normal); // Signed distance between sphere and plane

    if (dist < -sphere_radius || dist > sphere_radius)
    {
      // No intersection
      return false;
    }
  }

  // Step 2: Triangle-sphere check
  {
    vec3 scaled_normal;
    glm_vec3_scale(triangle_normal, dist, scaled_normal);

    vec3 point0;
    glm_vec3_sub(sphere_pos, scaled_normal, point0); // Projected sphere pos on triangle plane

    // Now determine whether point0 is inside all triangle edges

    vec3 v0, v1, v2;
    glm_vec3_sub(point0, triangle_p0, v0);
    glm_vec3_sub(point0, triangle_p1, v1);
    glm_vec3_sub(point0, triangle_p2, v2);

    vec3 w0, w1, w2;
    glm_vec3_sub(triangle_p1, triangle_p0, w0);
    glm_vec3_sub(triangle_p2, triangle_p1, w1);
    glm_vec3_sub(triangle_p0, triangle_p2, w2);

    vec3 c0, c1, c2;
    glm_vec3_cross(v0, w0, c0);
    glm_vec3_cross(v1, w1, c1);
    glm_vec3_cross(v2, w2, c2);

    bool inside = glm_vec3_dot(c0, triangle_normal) <= 0 && glm_vec3_dot(c1, triangle_normal) <= 0 &&
                  glm_vec3_dot(c2, triangle_normal) <= 0;

    float radius_sq = sphere_radius * sphere_radius; // Sphere radius squared

    bool intersects = false;
    vec3 point1, point2, point3;

    // Edge 1
    {
      closest_point_on_line(triangle_p1, triangle_p2, sphere_pos, point1);

      vec3 v;
      glm_vec3_sub(sphere_pos, point1, v);

      intersects = glm_vec3_dot(v, v) < radius_sq;
    }

    // Edge 2
    {
      closest_point_on_line(triangle_p0, triangle_p1, sphere_pos, point2);

      vec3 v;
      glm_vec3_sub(sphere_pos, point2, v);

      intersects |= glm_vec3_dot(v, v) < radius_sq;
    }

    // Edge 3
    {
      closest_point_on_line(triangle_p2, triangle_p0, sphere_pos, point3);

      vec3 v;
      glm_vec3_sub(sphere_pos, point3, v);

      intersects |= glm_vec3_dot(v, v) < radius_sq;
    }

    if (inside || intersects)
    {
      vec3 best_point;
      glm_vec3_copy(point0, best_point);

      vec3 intersection_vec;

      if (inside)
      {
        glm_vec3_sub(sphere_pos, point0, intersection_vec);
      }
      else
      {
        vec3 d;
        float best_dist_sq;

        {
          glm_vec3_sub(sphere_pos, point1, d);

          best_dist_sq = glm_vec3_dot(d, d);
          glm_vec3_copy(point1, best_point);
          glm_vec3_copy(d, intersection_vec);
        }

        {
          glm_vec3_sub(sphere_pos, point2, d);

          float dist_sq = glm_vec3_dot(d, d);
          if (dist_sq < best_dist_sq)
          {
            best_dist_sq = dist_sq;
            glm_vec3_copy(point2, best_point);
            glm_vec3_copy(d, intersection_vec);
          }
        }

        {
          glm_vec3_sub(sphere_pos, point3, d);

          float dist_sq = glm_vec3_dot(d, d);
          if (dist_sq < best_dist_sq)
          {
            glm_vec3_copy(point3, best_point);
            glm_vec3_copy(d, intersection_vec);
          }
        }
      }

      const float len = glm_vec3_norm(intersection_vec);
      glm_vec3_divs(intersection_vec, len, pen_normal);
      *pen_depth = sphere_radius - len;

      return true;
    }

    return false;
  }
}
