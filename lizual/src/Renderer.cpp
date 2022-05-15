#include "Renderer.h"

#include <iostream>

void GLClearError() noexcept {
  while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
  if (const GLenum error = glGetError()) {
    std::cout << "[OpenGL Error] ("
      << GL_ERROR_MAP.at(error) << "): "
      << function << " " << file << ":" << line << std::endl;
    return false;
  }
  return true;
}