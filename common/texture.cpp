#include "texture.hpp"

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

void Texture::load_from_file(const std::string& path) {
  filename = path.substr(path.rfind('/') + 1);

  glGenTextures(1, &id);

  int width = 0;
  int height = 0;
  int nr_components = 0;

  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &nr_components, 0);
  if (data != nullptr) {
    GLenum format = 0;
    if (nr_components == 1) {
      format = GL_RED;
    } else if (nr_components == 3) {
      format = GL_RGB;
    } else if (nr_components == 4) {
      format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    std::cerr << "Failed to load texture\n";
  }

  stbi_image_free(data);
}
