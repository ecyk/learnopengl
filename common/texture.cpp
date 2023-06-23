#include "texture.hpp"

#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>

Texture::Texture(const std::string& path)
    : filename_{std::filesystem::path{path}.filename().string()} {
  glGenTextures(1, &id_);

  int width{};
  int height{};
  int components{};

  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &components, 0);

  assert(data && "Failed to load texture from file.");

  GLenum format{};
  if (components == 1) {
    format = GL_RED;
  } else if (components == 3) {
    format = GL_RGB;
  } else if (components == 4) {
    format = GL_RGBA;
  }

  glBindTexture(GL_TEXTURE_2D, id_);
  glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0,
               format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
}

Texture::~Texture() {
  assert(id_ > 0 && "Invalid texture id.");
  glDeleteProgram(id_);
}

void Texture::bind() const { glBindTexture(GL_TEXTURE_2D, id_); }
