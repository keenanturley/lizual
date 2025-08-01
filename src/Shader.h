#pragma once

#include <glad/gl.h>
#include <glm/mat4x4.hpp>

#include <filesystem>
#include <string>

class Shader {
 public:
  Shader(
    const std::filesystem::path& vertexShaderPath,
    const std::filesystem::path& fragmentShaderPath
  );
  void Use();
  void SetBool(const std::string& name, bool value) const;
  void SetInt(const std::string& name, int value) const;
  float GetFloat(const std::string& name) const;
  void SetFloat(const std::string& name, float value) const;
  void SetUniform4f(
    const std::string& name, float v0, float v1, float v2, float v3
  ) const;
  void SetUniformMatrix4fv(const std::string& name, const glm::mat4& value);

 private:
  GLuint shaderProgram_;
};
