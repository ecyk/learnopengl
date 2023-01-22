#include "shader.hpp"

#include <glad/gl.h>

#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader(const std::filesystem::path& vertex_path,
               const std::filesystem::path& fragment_path) {
  assert(vertex_path.extension() == ".vert" &&
         fragment_path.extension() == ".frag");

  auto read_file = [](const std::filesystem::path& path) {
    std::ifstream file{path};
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    return std::string{std::istreambuf_iterator<char>{file},
                       std::istreambuf_iterator<char>{}};
  };

  const std::string vertex_code = read_file(vertex_path);
  const std::string fragment_code = read_file(fragment_path);

  auto create_shader = [](const char* shader_code, GLenum type,
                          std::string& buffer) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_code, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
      glGetShaderInfoLog(shader, static_cast<int>(buffer.size()), nullptr,
                         buffer.data());
      std::cerr << buffer << '\n';
    }
    return shader;
  };

  std::string buffer;
  buffer.resize(1024);

  const unsigned int vertex_shader =
      create_shader(vertex_code.c_str(), GL_VERTEX_SHADER, buffer);
  const unsigned int fragment_shader =
      create_shader(fragment_code.c_str(), GL_FRAGMENT_SHADER, buffer);

  const unsigned int program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == 0) {
    glGetProgramInfoLog(program, static_cast<int>(buffer.size()), nullptr,
                        buffer.data());
    std::cerr << buffer << '\n';
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  id_ = program;
}

void Shader::use() { glUseProgram(id_); }

void Shader::set_bool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
}

void Shader::set_int(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(id_, name.c_str()), 1,
               glm::value_ptr(value));
}

void Shader::set_mat4(const std::string& name, const glm::mat4& value) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}
