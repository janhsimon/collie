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

          const float dist_sq = glm_vec3_dot(d, d);
          if (dist_sq < best_dist_sq)
          {
            best_dist_sq = dist_sq;
            glm_vec3_copy(point2, best_point);
            glm_vec3_copy(d, intersection_vec);
          }
        }

        {
          glm_vec3_sub(sphere_pos, point3, d);

          const float dist_sq = glm_vec3_dot(d, d);
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

bool capsule_triangle_collision(vec3 capsule_base,
                                vec3 capsule_tip,
                                float capsule_radius,
                                vec3 triangle_p0,
                                vec3 triangle_p1,
                                vec3 triangle_p2,
                                vec3 triangle_normal,
                                vec3 pen_normal,
                                float* pen_depth)
{
  vec3 CapsuleNormal;
  glm_vec3_sub(capsule_tip, capsule_base, CapsuleNormal);
  glm_vec3_normalize(CapsuleNormal);

  vec3 LineEndOffset;
  glm_vec3_scale(CapsuleNormal, capsule_radius, LineEndOffset);

  vec3 A, B;
  glm_vec3_add(capsule_base, LineEndOffset, A);
  glm_vec3_sub(capsule_tip, LineEndOffset, B);

  // Then for each triangle, ray-plane intersection:
  //  N is the triangle plane normal (it was computed in sphere – triangle intersection case)

  vec3 a;
  glm_vec3_sub(triangle_p0, capsule_base, a);

  float t = glm_dot(triangle_normal, a) / fabs(glm_dot(triangle_normal, CapsuleNormal));

  vec3 line_plane_intersection;
  vec3 aa;
  glm_vec3_scale(CapsuleNormal, t, aa);
  glm_vec3_add(capsule_base, aa, line_plane_intersection);

  // Determine whether line_plane_intersection is inside all triangle edges:
  vec3 reference_point;
  {
    vec3 c0, c1, c2;

    {
      vec3 aaa, bbb;
      glm_vec3_sub(line_plane_intersection, triangle_p0, aaa);
      glm_vec3_sub(triangle_p1, triangle_p0, bbb);
      glm_cross(aaa, bbb, c0);
    }

    {
      vec3 aaa, bbb;
      glm_vec3_sub(line_plane_intersection, triangle_p1, aaa);
      glm_vec3_sub(triangle_p2, triangle_p1, bbb);
      glm_cross(aaa, bbb, c1);
    }

    {
      vec3 aaa, bbb;
      glm_vec3_sub(line_plane_intersection, triangle_p2, aaa);
      glm_vec3_sub(triangle_p0, triangle_p2, bbb);
      glm_cross(aaa, bbb, c2);
    }

    bool inside =
      glm_dot(c0, triangle_normal) <= 0 && glm_dot(c1, triangle_normal) <= 0 && glm_dot(c2, triangle_normal) <= 0;

    if (inside)
    {
      glm_vec3_copy(line_plane_intersection, reference_point);
    }
    else
    {
      float best_dist;

      // Edge 1:
      {
        vec3 point1;
        closest_point_on_line(triangle_p0, triangle_p1, line_plane_intersection, point1);
        vec3 v1;
        glm_vec3_sub(line_plane_intersection, point1, v1);
        const float distsq = glm_dot(v1, v1);
        best_dist = distsq;
        glm_vec3_copy(point1, reference_point);
      }

      // Edge 2:
      {
        vec3 point2;
        closest_point_on_line(triangle_p1, triangle_p2, line_plane_intersection, point2);
        vec3 v2;
        glm_vec3_sub(line_plane_intersection, point2, v2);
        const float distsq = glm_dot(v2, v2);
        if (distsq < best_dist)
        {
          glm_vec3_copy(point2, reference_point);
          best_dist = distsq;
        }
      }

      // Edge 2:
      {
        vec3 point3;
        closest_point_on_line(triangle_p2, triangle_p0, line_plane_intersection, point3);
        vec3 v3;
        glm_vec3_sub(line_plane_intersection, point3, v3);
        const float distsq = glm_dot(v3, v3);
        if (distsq < best_dist)
        {
          glm_vec3_copy(point3, reference_point);
          // best_dist = distsq;
        }
      }
    }
  }

  // The center of the best sphere candidate:
  vec3 center;
  closest_point_on_line(A, B, reference_point, center);

  return sphere_triangle_collision(center, capsule_radius, triangle_p0, triangle_p1, triangle_p2, triangle_normal,
                                   pen_normal, pen_depth);
}