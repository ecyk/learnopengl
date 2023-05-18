#include "model.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/gl.h>

#include <assimp/Importer.hpp>
#include <iostream>

Model::Model(const std::filesystem::path& path)
    : directory_{path.parent_path().string()} {
  load_model(path);
}

Model::~Model() {
  for (const auto& texture : loaded_textures_) {
    glDeleteTextures(1, &texture.id);
  }
}

void Model::draw(Shader& shader) const {
  for (const auto& mesh : meshes_) {
    mesh.draw(shader);
  }
}

void Model::load_model(const std::filesystem::path& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

  if ((scene == nullptr) ||
      ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U) ||
      (scene->mRootNode == nullptr)) {
    std::cout << importer.GetErrorString() << '\n';
    return;
  }

  process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(process_mesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    process_node(node->mChildren[i], scene);
  }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex{};

    vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y,
                       mesh->mVertices[i].z};
    vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y,
                     mesh->mNormals[i].z};
    vertex.tex_coords = mesh->mTextureCoords[0] != nullptr
                            ? glm::vec2{mesh->mTextureCoords[0][i].x,
                                        mesh->mTextureCoords[0][i].y}
                            : glm::vec2{};

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuse_maps =
        load_material_textures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<Texture> specular_maps =
        load_material_textures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
  }

  return {vertices, indices, textures};
}

std::vector<Texture> Model::load_material_textures(aiMaterial* material,
                                                   aiTextureType type) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
    aiString string;
    material->GetTexture(type, i, &string);

    if (auto it = std::find_if(loaded_textures_.begin(), loaded_textures_.end(),
                               [&](const Texture& texture) {
                                 return texture.filename == string.C_Str();
                               });
        it != loaded_textures_.end()) {
      textures.push_back(*it);
      continue;
    }

    Texture texture;
    texture.load_from_file(directory_ + '/' + std::string{string.C_Str()});

    switch (type) {
      case aiTextureType_DIFFUSE:
        texture.type = Texture::Type::DIFFUSE;
        break;
      case aiTextureType_SPECULAR:
        texture.type = Texture::Type::SPECULAR;
        break;
      default:
        break;
    }

    textures.push_back(texture);
    loaded_textures_.push_back(texture);
  }

  return textures;
}
