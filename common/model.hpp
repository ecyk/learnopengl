#pragma once

#include <assimp/material.h>

#include <memory>
#include <unordered_map>

#include "mesh.hpp"

struct aiMaterial;
struct aiNode;
struct aiMesh;
struct aiScene;

class Model {
 public:
  explicit Model(const std::string& path);

  void draw(Shader& shader) const;

 private:
  std::vector<std::shared_ptr<Texture>> loaded_textures_;
  std::vector<Mesh> meshes_;
  std::string directory_;

  void load_model(const std::string& path);
  void process_node(aiNode* node, const aiScene* scene);
  Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
  std::vector<std::shared_ptr<Texture>> load_material_textures(
      aiMaterial* material, aiTextureType type);
};
