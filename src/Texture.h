#pragma once

#include <glad/gl.h>
#include <stb_image.h>

#include <filesystem>
#include <memory>

class Texture {
 public:
  // Loads a file from disk. Returns nullptr if unsuccessful
  static std::unique_ptr<Texture> Load(const std::filesystem::path& imagePath);

  // Remove copy and assignment constructors
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  const GLuint textureId;

 private:
  Texture(GLuint textureId) : textureId(textureId) {}
};
