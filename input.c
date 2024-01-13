#include "input.h"

#include "window.h"

#include <cglm/vec2.h>

#include <glfw/glfw3.h>

#include <stdint.h>

#define LEFT_MOUSE_BUTTON (1 << 0)
#define RIGHT_MOUSE_BUTTON (1 << 1)

static vec2 cursor_pos, last_cursor_pos;
static float scroll_pos;
static uint8_t mouse_button_mask = 0;
static uint8_t keyboard_key_mask = 0;

void init_input()
{
  // Initialize cursor and last cursor position
  {
    get_cursor_pos(cursor_pos);
    glm_vec2_copy(cursor_pos, last_cursor_pos);
  }

  // Initialize scroll position
  scroll_pos = 0.0f;
}

void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
  cursor_pos[0] = (float)x;
  cursor_pos[1] = (float)y;
}

void scroll_callback(GLFWwindow* window, double x, double y)
{
  scroll_pos += (float)y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  // Store mouse button state
  if (button == GLFW_MOUSE_BUTTON_LEFT)
  {
    if (action == GLFW_PRESS)
    {
      mouse_button_mask |= LEFT_MOUSE_BUTTON;
    }
    else
    {
      mouse_button_mask &= ~LEFT_MOUSE_BUTTON;
    }
  }
  else if (button == GLFW_MOUSE_BUTTON_RIGHT)
  {
    if (action == GLFW_PRESS)
    {
      mouse_button_mask |= RIGHT_MOUSE_BUTTON;
    }
    else
    {
      mouse_button_mask &= ~RIGHT_MOUSE_BUTTON;
    }
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_W)
  {
    if (action == GLFW_PRESS)
    {
      keyboard_key_mask |= KEY_W;
    }
    else if (action == GLFW_RELEASE)
    {
      keyboard_key_mask &= ~KEY_W;
    }
  }
  else if (key == GLFW_KEY_A)
  {
    if (action == GLFW_PRESS)
    {
      keyboard_key_mask |= KEY_A;
    }
    else if (action == GLFW_RELEASE)
    {
      keyboard_key_mask &= ~KEY_A;
    }
  }
  else if (key == GLFW_KEY_S)
  {
    if (action == GLFW_PRESS)
    {
      keyboard_key_mask |= KEY_S;
    }
    else if (action == GLFW_RELEASE)
    {
      keyboard_key_mask &= ~KEY_S;
    }
  }
  else if (key == GLFW_KEY_D)
  {
    if (action == GLFW_PRESS)
    {
      keyboard_key_mask |= KEY_D;
    }
    else if (action == GLFW_RELEASE)
    {
      keyboard_key_mask &= ~KEY_D;
    }
  }
  else if (key == GLFW_KEY_ESCAPE)
  {
    if (action == GLFW_PRESS)
    {
      keyboard_key_mask |= KEY_ESC;
    }
    else if (action == GLFW_RELEASE)
    {
      keyboard_key_mask &= ~KEY_ESC;
    }
  }
}

void get_cursor_delta(vec2 delta)
{
  glm_vec2_copy(cursor_pos, delta);
  glm_vec2_sub(delta, last_cursor_pos, delta);

  // Retrieve the inverse of the window size
  vec2 window_size, inv_window_size;
  get_window_size(window_size);
  glm_vec2_div(GLM_VEC2_ONE, window_size, inv_window_size);

  // Normalize the delta
  glm_vec2_mul(delta, inv_window_size, delta);

  glm_vec2_copy(cursor_pos, last_cursor_pos);
}

float get_scroll_delta()
{
  const float delta = scroll_pos;
  scroll_pos = 0.0f;
  return delta;
}

bool is_key_down(int key)
{
  return (keyboard_key_mask & key) == key;
}
