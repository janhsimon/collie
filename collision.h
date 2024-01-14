#pragma once

#include <cglm/types.h>

#include <stdbool.h>

bool sphere_triangle_collision(vec3 sphere_pos,
                               float sphere_radius,
                               vec3 triangle_p0,
                               vec3 triangle_p1,
                               vec3 triangle_p2,
                               vec3 triangle_normal,
                               vec3 pen_normal,
                               float* pen_depth);

bool capsule_triangle_collision(vec3 capsule_base,
                                vec3 capsule_tip,
                                float capsule_radius,
                                vec3 triangle_p0,
                                vec3 triangle_p1,
                                vec3 triangle_p2,
                                vec3 triangle_normal,
                                vec3 pen_normal,
                                float* pen_depth);
