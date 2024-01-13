#pragma once

#include <cglm/types.h>

#include <stdbool.h>

#define KEY_W (1 << 0)
#define KEY_A (1 << 1)
#define KEY_S (1 << 2)
#define KEY_D (1 << 3)
#define KEY_ESC (1 << 4)

struct GLFWwindow;

void init_input();

void cursor_pos_callback(struct GLFWwindow* window, double x, double y);
void scroll_callback(struct GLFWwindow* window, double x, double y);
void mouse_button_callback(struct GLFWwindow* window, int button, int action, int mods);
void key_callback(struct GLFWwindow* window, int key, int scancode, int action, int mods);

void get_cursor_delta(vec2 delta);
float get_scroll_delta();

bool is_key_down(int key);
