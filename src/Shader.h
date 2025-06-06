#pragma once

#include <glad/gl.h>

#include <string>

class Shader {
 public:
  Shader(
    const std::string& vertexShaderPath, const std::string& fragmentShaderPath
  );
  void Use();
  void SetBool(const std::string& name, bool value) const;
  void SetInt(const std::string& name, int value) const;
  void SetFloat(const std::string& name, float value) const;
  void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) const;

 private:
  GLuint shaderProgram_;
};
