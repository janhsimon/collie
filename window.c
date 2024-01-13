#include "window.h"

#include "camera.h"
#include "input.h"

#include <glad/gl.h>

#include <glfw/glfw3.h>

#include <stdio.h>

#define WINDOW_TITLE "Collie"

static GLFWwindow* window;

static int width = 1280;
static int height = 800;

static void window_resize_callback(GLFWwindow* window, int width_, int height_)
{
  width = width_;
  height = height_;

  // Update the camera projection matrix
  const float aspect = (float)width / (float)height;
  update_proj_matrix(aspect);
}

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

bool generate_window()
{
  if (!glfwInit())
  {
    printf("Failed to initialize GLFW");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  window = glfwCreateWindow(width, height, WINDOW_TITLE, NULL, NULL);
  if (!window)
  {
    printf("Failed to create window");
    destroy_window();
    return false;
  }

  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetWindowSizeCallback(window, window_resize_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwRawMouseMotionSupported())
  {
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }

  glfwMakeContextCurrent(window);
  if (gladLoadGL(glfwGetProcAddress) == 0)
  {
    printf("Failed to load OpenGL");
    destroy_window();
    return false;
  }

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glEnable(GL_MULTISAMPLE);

  window_resize_callback(window, width, height);

  return true;
}

void destroy_window()
{
  glfwTerminate();
}

void get_window_size(vec2 size)
{
  int window_width, window_height;
  glfwGetWindowSize(window, &window_width, &window_height);

  size[0] = (float)window_width;
  size[1] = (float)window_height;
}

void get_cursor_pos(vec2 cursor_pos)
{
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  cursor_pos[0] = (float)x;
  cursor_pos[1] = (float)y;
}

double get_time()
{
  return glfwGetTime();
}

void clear_window()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void refresh_window()
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

bool should_window_close()
{
  return glfwWindowShouldClose(window);
}

void request_window_close()
{
  glfwSetWindowShouldClose(window, GLFW_TRUE);
}
