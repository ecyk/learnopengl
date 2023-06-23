#pragma once

#include <string>

class Texture {
 public:
  enum class Type { None, Diffuse, Specular };

  explicit Texture(const std::string& path);

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  Texture(Texture&&) = default;
  Texture& operator=(Texture&&) = default;

  ~Texture();

  void bind() const;

  [[nodiscard]] Type get_type() const { return type_; }
  void set_type(Type type) { type_ = type; }
  [[nodiscard]] const std::string& get_filename() const { return filename_; }

 private:
  unsigned int id_{};
  Type type_{};
  std::string filename_;
};
