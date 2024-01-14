#pragma once

#include <cglm/types.h>

#include <stdbool.h>

bool generate_player();
void destroy_player();

mat4* get_player_transform();
float get_player_height();

void update_player(const vec2 cursor_delta, float delta_time);
void draw_player(const mat4 viewproj_matrix);
