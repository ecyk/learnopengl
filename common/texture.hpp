#pragma once

#include <string>

struct Texture {
  enum class Type { NONE, DIFFUSE, SPECULAR };

  void load_from_file(const std::string& path);

  unsigned int id{};
  Type type{};
  std::string filename;
};
