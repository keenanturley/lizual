#define SDL_MAIN_USE_CALLBACKS

#include <cstdint>
#include <memory>
#include <fstream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <glad/gl.h>
#include <stb_image.h>

#include "Shader.h"

namespace {
constexpr int kDefaultWindowWidth = 640;
constexpr int kDefaultWindowHeight = 480;
const std::filesystem::path kAssetsDir = LIZUAL_ASSETS_DIR;
const std::filesystem::path kVertexShaderPath = kAssetsDir / "shaders/default.vert";
const std::filesystem::path kFragmentShaderPath = kAssetsDir / "shaders/default.frag";
const std::filesystem::path kContainerTexturePath = kAssetsDir / "textures/container.jpg";
const std::filesystem::path kAwesomeFaceTexturePath = kAssetsDir / "textures/awesomeface.png";

// clang-format off
// Vertices for a rectangle
constexpr float kVertices[] = {
  // positions        // colors         // texture coords
   0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
   0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
  -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left 
};

constexpr uint32_t kIndices[] = {
  0, 1, 3, // first triangle
  1, 2, 3  // second triangle
};
// clang-format on
}  // namespace

struct AppState {
  SDL_Window *window;
  SDL_GLContext glContext;
  Shader* shader;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s", SDL_GetError()
    );
    return SDL_APP_FAILURE;
  }
  SDL_Log("SDL initialized successfully");
  
  // Limit FPS temporarily so my laptop doesn't burn my legs
  SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");

  // Set OpenGL version attributes, necessary for MacOSX, otherwise it will
  // default to OpenGL 2.1 and segfault on use of functions not available in 2.1
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *window = SDL_CreateWindow(
    "Lizual",
    kDefaultWindowWidth,
    kDefaultWindowHeight,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );
  if (window == nullptr) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR,
      "SDL_CreateWindowAndRenderer failed: %s",
      SDL_GetError()
    );
    return SDL_APP_FAILURE;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (glContext == nullptr) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "SDL_GL_CreateContext failed: %s", SDL_GetError()
    );
    return SDL_APP_FAILURE;
  }

  int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  if (version == 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to load GLAD");
    return SDL_APP_FAILURE;
  }
  const int major = GLAD_VERSION_MAJOR(version);
  const int minor = GLAD_VERSION_MINOR(version);
  SDL_Log("OpenGL version: %d.%d", major, minor);
  SDL_Log("  Full version string: %s", glGetString(GL_VERSION));

  // Set the default viewport size
  int widthInPixels;
  int heightInPixels;
  if (!SDL_GetWindowSizeInPixels(window, &widthInPixels, &heightInPixels)) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR,
      "SDL_GetWindowSizeInPixels failed: %s",
      SDL_GetError()
    );
    return SDL_APP_FAILURE;
  }
  glViewport(0, 0, widthInPixels, heightInPixels);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  // Create a vertex array object (VAO) for the rectangle
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a vertex buffer object (VBO) for the rectangle
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

  // Element buffer object (EBO) for the rectangle indices
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW
  );

  // Create the shader
  // remember to have a try catch block for handling file read exceptions
  Shader* shader; 
  try {
    shader = new Shader(kVertexShaderPath, kFragmentShaderPath);
  } catch (const std::ifstream::failure& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to read shader file: %s", e.what());
    return SDL_APP_FAILURE;
  } catch (const std::runtime_error& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "Failed to create shader: %s", e.what());
    return SDL_APP_FAILURE;
  }
  shader->Use();

  // Set the vertex attribute pointers
  int stride = 8 * sizeof(float);
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(0);
  // color
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float))
  );
  glEnableVertexAttribArray(1);
  // texture coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Load the container texture
  SDL_Log("Loading container texture");
  int width, height, numChannels;
  unsigned char* data = stbi_load(kContainerTexturePath.c_str(), &width, &height, &numChannels, 0);
  if (data == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[stb_image] Failed to load texture from path %s", kContainerTexturePath.c_str());
    return SDL_APP_FAILURE;
  } 
  SDL_Log("  Loaded container texture: %d x %d", width, height);

  // Create an OpenGL texture
  GLuint textureId;
  glGenTextures(1, &textureId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  shader->SetInt("uTexture", 0);
  
  // Free the image
  stbi_image_free(data);

  // Load the awesome face texture
  SDL_Log("Loading awesome face texture");
  // Flip vertically because it's inversed by default
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data2 = stbi_load(kAwesomeFaceTexturePath.c_str(), &width, &height, &numChannels, 0);
  stbi_set_flip_vertically_on_load(false);
  if (data == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "[stb_image] Failed to load texture from path %s", kContainerTexturePath.c_str());
    return SDL_APP_FAILURE;
  } 
  SDL_Log("  Loaded awesome face texture: %d x %d", width, height);

  // Create an OpenGL texture
  GLuint textureId2;
  glGenTextures(1, &textureId2);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureId2);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
  glGenerateMipmap(GL_TEXTURE_2D);
  shader->SetInt("uTexture2", 1);
  
  // Free the image
  stbi_image_free(data2);

  *appstate = new AppState{
    window,
    glContext,
    shader,
  };
  SDL_Log("App initialization complete");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);

  glClearColor(0.75f, 0.75f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Time is seconds since the start of the program
  float time = SDL_GetTicks() / 1000.0f;
  state->shader->SetFloat("uTime", time);

  // Draw the triangle
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  SDL_GL_SwapWindow(state->window);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = static_cast<AppState *>(appstate);

  if (event->type == SDL_EVENT_QUIT) {
    SDL_Log("Received quit event");
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    int widthInPixels;
    int heightInPixels;
    if (!SDL_GetWindowSizeInPixels(
          state->window, &widthInPixels, &heightInPixels
        )) {
      SDL_LogCritical(
        SDL_LOG_CATEGORY_ERROR,
        "SDL_GetWindowSizeInPixels failed: %s",
        SDL_GetError()
      );
      return SDL_APP_FAILURE;
    }
    glViewport(0, 0, widthInPixels, heightInPixels);
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = static_cast<AppState *>(appstate);

  delete state->shader;

  SDL_Log("Exiting with result: %d", result);
  SDL_GL_DestroyContext(state->glContext);
  SDL_DestroyWindow(state->window);

  delete state;
  SDL_Quit();
}
