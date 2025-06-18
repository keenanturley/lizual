#include <SDL3/SDL_log.h>
#include <stb_image.h>

#include "Texture.h"

std::unique_ptr<Texture> Texture::Load(const std::filesystem::path& imagePath) {
  SDL_Log("Loading texture from path: %s", imagePath.c_str());
  int width, height, numChannels;
  unsigned char* data =
    stbi_load(imagePath.c_str(), &width, &height, &numChannels, 0);
  if (data == nullptr) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_APPLICATION,
      "[stb_image] Failed to load texture from path %s",
      imagePath.c_str()
    );
    return nullptr;
  }
  SDL_Log(
    "  Loaded texture file from disk: %d x %d, numChannels: %d",
    width,
    height,
    numChannels
  );

  // Create an OpenGL texture
  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  // Determine format from channels
  GLenum format;
  switch (numChannels) {
    // note(keenan): Revisit cases 1 and 2 to spread gray across the
    // proper channels
    case 1:  // Gray
      format = GL_RED;
      break;
    case 2:  // Gray, Alpha
      format = GL_RG;
      break;
    case 3:  // RGB
      format = GL_RGB;
      break;
    case 4:  // RGBA
      format = GL_RGBA;
      break;
    default:
      SDL_LogCritical(
        SDL_LOG_CATEGORY_APPLICATION,
        "Unsupported number of channels: %d",
        numChannels
      );
      return nullptr;
  }

  // Load the texture data into OpenGL
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data
  );
  glGenerateMipmap(GL_TEXTURE_2D);

  // Free image memory
  stbi_image_free(data);

  return std::unique_ptr<Texture>(new Texture(textureId));
}
