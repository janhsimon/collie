#include <glad/gl.h>
#include <glfw/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

static const char window_title[] = "Collie";
static int window_width = 1280;
static int window_height = 800;

int main(int argc, char* argv[])
{
  // Create window and load OpenGL
  GLFWwindow* window;
  {
    if (!glfwInit())
    {
      printf("Failed to initialize GLFW");
      return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
    if (!window)
    {
      printf("Failed to create window");
      glfwTerminate();
      return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    if (gladLoadGL(glfwGetProcAddress) == 0)
    {
      printf("Failed to load OpenGL");
      return EXIT_FAILURE;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_MULTISAMPLE);
  }

  // double prev_time = glfwGetTime(); // In seconds

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    // Update
    {
      // Calculate delta time
      /*
      const double now_time = glfwGetTime(); // In seconds
      const float delta_time = (float)(now_time - prev_time);
      prev_time = now_time;
      */
    }

    // Render
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}