#include "Shader.h"

#include <SDL3/SDL_log.h>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(
  const std::filesystem::path& vertexShaderPath,
  const std::filesystem::path& fragmentShaderPath
) {
  // 1. Retrieve the vertex and fragment source from the file paths
  std::string vertexShaderSource;
  std::string fragmentShaderSource;
  std::ifstream vertexShaderFile;
  std::ifstream fragmentShaderFile;
  vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  vertexShaderFile.open(vertexShaderPath);
  fragmentShaderFile.open(fragmentShaderPath);
  std::stringstream vertexShaderStream, fragmentShaderStream;
  vertexShaderStream << vertexShaderFile.rdbuf();
  fragmentShaderStream << fragmentShaderFile.rdbuf();
  vertexShaderFile.close();
  fragmentShaderFile.close();
  vertexShaderSource = vertexShaderStream.str();
  fragmentShaderSource = fragmentShaderStream.str();
  const char* vertexShaderSourceCString = vertexShaderSource.c_str();
  const char* fragmentShaderSourceCString = fragmentShaderSource.c_str();

  // Create the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSourceCString, nullptr);
  glCompileShader(vertexShader);

  // Check for compilation errors
  GLint success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    throw std::runtime_error(
      std::format("GL: Vertex shader compilation failed: {}", infoLog)
    );
  }

  // Create the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSourceCString, nullptr);
  glCompileShader(fragmentShader);

  // Check for compilation errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    throw std::runtime_error(
      std::format("GL: Fragment shader compilation failed: {}", infoLog)
    );
  }

  // Create the shader program
  shaderProgram_ = glCreateProgram();
  glAttachShader(shaderProgram_, vertexShader);
  glAttachShader(shaderProgram_, fragmentShader);
  glLinkProgram(shaderProgram_);

  // Check for linking errors
  glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
    throw std::runtime_error(
      std::format("GL: Shader program linking failed: {}", infoLog)
    );
  }

  // Delete the shaders as they're linked into the program now
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::Use() { glUseProgram(shaderProgram_); }

void Shader::SetBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(shaderProgram_, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(shaderProgram_, name.c_str()), value);
}

float Shader::GetFloat(const std::string& name) const {
  GLint location = glGetUniformLocation(shaderProgram_, name.c_str());
  if (location == -1) {
    throw std::runtime_error(std::format("GL: Uniform {} not found", name));
  }
  float value;
  glGetUniformfv(shaderProgram_, location, &value);
  return value;
}

void Shader::SetFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(shaderProgram_, name.c_str()), value);
}

void Shader::SetUniform4f(
  const std::string& name, float v0, float v1, float v2, float v3
) const {
  glUniform4f(
    glGetUniformLocation(shaderProgram_, name.c_str()), v0, v1, v2, v3
  );
}
