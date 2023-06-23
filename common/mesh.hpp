#pragma once

#include <memory>
#include <vector>

#include "shader.hpp"
#include "texture.hpp"

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

class Mesh {
 public:
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<std::shared_ptr<Texture>> textures);

  void draw(Shader& shader) const;

 private:
  void setup();

  unsigned int vao_{}, vbo_{}, ebo_{};

  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<std::shared_ptr<Texture>> textures_;
};
