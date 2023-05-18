#include "mesh.hpp"

#include <glad/gl.h>

#include <string>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures)
    : vertices_{std::move(vertices)},
      indices_{std::move(indices)},
      textures_{std::move(textures)} {
  setup();
}

void Mesh::draw(Shader& shader) const {
  unsigned int diffuse = 1;
  unsigned int specular = 1;

  std::string buffer;
  buffer.resize(64);

  for (int i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    switch (textures_[i].type) {
      case Texture::Type::DIFFUSE:
        buffer = "material.texture_diffuse" + std::to_string(diffuse++);
        break;
      case Texture::Type::SPECULAR:
        buffer = "material.texture_specular" + std::to_string(specular++);
        break;
      default:
        break;
    }

    shader.set_int(buffer, i);
    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Mesh::setup() {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               vertices_.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
               indices_.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, position)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, normal)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, tex_coords)));

  glBindVertexArray(0);
}
