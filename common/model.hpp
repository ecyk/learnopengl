#pragma once

#include <assimp/material.h>

#include <unordered_map>

#include "mesh.hpp"

struct aiMaterial;
struct aiNode;
struct aiMesh;
struct aiScene;

class Model {
 public:
  explicit Model(const std::filesystem::path& path);
  ~Model();

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
  Model(Model&& other) = default;
  Model& operator=(Model&& other) = default;

  void draw(Shader& shader) const;

 private:
  std::vector<Texture> loaded_textures_;
  std::vector<Mesh> meshes_;

  std::string directory_;

  void load_model(const std::filesystem::path& path);
  void process_node(aiNode* node, const aiScene* scene);
  Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> load_material_textures(aiMaterial* material,
                                              aiTextureType type);
};
