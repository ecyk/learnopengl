#pragma once

#include <filesystem>
#include <glm/glm.hpp>

class Shader {
 public:
  Shader(const std::filesystem::path& vertex_path,
         const std::filesystem::path& fragment_path);

  void use();

  void set_bool(const std::string& name, bool value) const;
  void set_int(const std::string& name, int value) const;
  void set_float(const std::string& name, float value) const;

  void set_vec3(const std::string& name, const glm::vec3& value) const;

  void set_mat4(const std::string& name, const glm::mat4& value) const;

  unsigned int id_{};
};
