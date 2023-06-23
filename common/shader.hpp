#pragma once

#include <glm/glm.hpp>
#include <string_view>

class Shader {
 public:
  Shader(const std::string& vertex_path, const std::string& fragment_path);

  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  Shader(Shader&&) = default;
  Shader& operator=(Shader&&) = default;

  ~Shader();

  void use() const;

  void set_bool(const std::string& name, bool value) const;
  void set_int(const std::string& name, int value) const;
  void set_float(const std::string& name, float value) const;

  void set_vec3(const std::string& name, const glm::vec3& value) const;

  void set_mat4(const std::string& name, const glm::mat4& value) const;

  [[nodiscard]] unsigned int get_id() const { return id_; }

 private:
  unsigned int id_{};
};
