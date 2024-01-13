#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdio.h>

bool load_texture(const char* filename, GLuint* texture)
{
  int width, height, channels;
  stbi_uc* data = stbi_load(filename, &width, &height, &channels, 0);
  if (!data)
  {
    printf("Failed to load texture \"%s\"\n", filename);
    return false;
  }

  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format;
  if (channels == 1)
  {
    format = GL_RED;
  }
  else if (channels == 3)
  {
    format = GL_RGB;
  }
  else if (channels == 4)
  {
    format = GL_RGBA;
  }
  else
  {
    printf("Texture \"%s\" has invalid channel count %d, expected 1, 3 or 4\n", filename, channels);
    stbi_image_free(data);
    return false;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  return true;
}

void destroy_texture(GLuint texture)
{
  glDeleteTextures(1, &texture);
}
