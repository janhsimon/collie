#pragma once

#include <cglm/types.h>

#include <stdbool.h>

bool generate_window();
void destroy_window();

void get_window_size(vec2 size);
void get_cursor_pos(vec2 cursor_pos);
double get_time(); // In seconds

void clear_window();
void refresh_window();

bool should_window_close();
void request_window_close();