#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#define SHADER_LOG_SIZE 512

bool load_shader(const char* filename, GLenum type, GLuint* shader)
{
  FILE* file = fopen(filename, "rb");
  if (!file)
  {
    printf("Failed to load shader \"%s\"\n", filename);
    return false;
  }

  fseek(file, 0, SEEK_END);
  GLint length = (GLint)ftell(file);
  if (length <= 0)
  {
    printf("Shader \"%s\" has unexpected length %d\n", filename, length);
    fclose(file);
    return false;
  }

  fseek(file, 0, SEEK_SET);

  GLchar* source = malloc(length);
  if (!source)
  {
    printf("Ran out of memory while loading shader \"%s\", requested %d bytes for source", filename, length);
    fclose(file);
    return false;
  }

  fread(source, length, 1, file);
  fclose(file);

  *shader = glCreateShader(type);

  const GLchar* sources[] = { source };
  glShaderSource(*shader, 1, sources, &length);
  free(source);

  glCompileShader(*shader);

  GLint success;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    GLchar log[SHADER_LOG_SIZE];
    glGetShaderInfoLog(*shader, SHADER_LOG_SIZE, NULL, log);
    printf("Failed to compile shader:\n%s", log);
    return false;
  }

  return true;
}

void destroy_shader(GLuint shader_program)
{
  glDeleteProgram(shader_program);
}

bool generate_shader_program(GLuint vertex_shader, GLuint fragment_shader, GLuint* shader_program)
{
  *shader_program = glCreateProgram();
  glAttachShader(*shader_program, vertex_shader);
  glAttachShader(*shader_program, fragment_shader);

  glLinkProgram(*shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  GLint success;
  glGetProgramiv(*shader_program, GL_LINK_STATUS, &success);
  if (!success)
  {
    GLchar log[SHADER_LOG_SIZE];
    glGetProgramInfoLog(*shader_program, SHADER_LOG_SIZE, NULL, log);
    printf("Failed to link shader program:\n%s", log);
    return false;
  }

  return true;
}

GLint get_uniform_location(GLuint shader_program, const char* name)
{
  GLint uniform_location = glGetUniformLocation(shader_program, name);
  if (shader_program < 0)
  {
    printf("Failed to get uniform location for %s", name);
  }

  return uniform_location;
}

void set_uniform_i(GLuint shader_program, const char* name, int value)
{
  const GLint uniform_location = get_uniform_location(shader_program, name);
  glUniform1i(uniform_location, value);
}

void set_uniform_vec4(GLuint shader_program, const char* name, const vec4 value)
{
  const GLint uniform_location = get_uniform_location(shader_program, name);
  glUniform4fv(uniform_location, 1, value);
}
