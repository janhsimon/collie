#pragma once

#include <cglm/types.h>

void update_proj_matrix(float aspect);
void update_camera(const vec2 cursor_delta,
                   float scroll_delta,
                   mat4 player_transform,
                   float player_height,
                   mat4 viewproj_matrix);
