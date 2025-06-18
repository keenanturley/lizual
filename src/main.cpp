#define SDL_MAIN_USE_CALLBACKS

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>

#include <algorithm>
#include <cstdint>
#include <memory>

#include "Camera.h"
#include "Renderer.h"

namespace {
constexpr int kDefaultWindowWidth = 640;
constexpr int kDefaultWindowHeight = 480;
}  // namespace

struct AppState {
  SDL_Window* window;
  // Previous tick (Nanoseconds since SDL was initialized) that was processed by
  // the frameloop
  uint64_t previousTickNs;
  uint64_t previousFrameTimeNs;
  SDL_GLContext glContext;
  std::unique_ptr<Camera> camera;
  std::unique_ptr<Renderer> renderer;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
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

  SDL_Window* window = SDL_CreateWindow(
    "Lizual",
    kDefaultWindowWidth,
    kDefaultWindowHeight,
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );
  if (window == nullptr) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow failed: %s", SDL_GetError()
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

  std::unique_ptr<Renderer> renderer = Renderer::Create(window, glContext);

  // Configure Camera
  std::unique_ptr camera =
    std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

  uint64_t lastTick = SDL_GetTicksNS();
  *appstate = new AppState{
    window,
    lastTick,
    static_cast<uint64_t>(0),
    glContext,
    std::move(camera),
    std::move(renderer)
  };
  SDL_Log("App initialization complete");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  const uint64_t perfCounterStart = SDL_GetPerformanceCounter();
  AppState* state = static_cast<AppState*>(appstate);
  const uint64_t currentTickNs = SDL_GetTicksNS();
  const float currentTickSeconds =
    static_cast<float>(currentTickNs) / static_cast<float>(SDL_NS_PER_SECOND);
  const uint64_t deltaTicksNs = currentTickNs - state->previousTickNs;
  const float deltaTimeSeconds =
    static_cast<float>(deltaTicksNs) / static_cast<float>(SDL_NS_PER_SECOND);

  // -- Get Input
  const bool* keys = SDL_GetKeyboardState(nullptr);

  // -- Update state
  // Update Camera based on input
  // TODO: Refactor this into a KeyboardCameraController class or something
  if (state->camera == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Camera is null");
    return SDL_APP_FAILURE;
  }
  Camera& camera = *state->camera;
  // Sensitivity in degrees per second
  const float cameraSensitivity = 40.0f;
  // XY(pitch,yaw) rotation delta in degrees
  glm::vec2 rotationDelta{
    (-1 * keys[SDL_SCANCODE_DOWN]) + keys[SDL_SCANCODE_UP],
    (-1 * keys[SDL_SCANCODE_RIGHT]) + keys[SDL_SCANCODE_LEFT]
  };
  // Apply magnitude
  rotationDelta *= cameraSensitivity * deltaTimeSeconds;
  camera.Rotate(rotationDelta);

  // Update Camera position with WASD + Q/E for down/up
  // Units per second
  float speed = 2.0f;
  // XYZ direction
  glm::vec3 positionDelta{
    (-1 * keys[SDL_SCANCODE_A]) + keys[SDL_SCANCODE_D],
    (-1 * keys[SDL_SCANCODE_Q]) + keys[SDL_SCANCODE_E],
    (-1 * keys[SDL_SCANCODE_W]) + keys[SDL_SCANCODE_S],
  };
  camera.Move(positionDelta * speed * deltaTimeSeconds);

  // -- Render
  float frameTimeMs =
    state->previousFrameTimeNs / static_cast<float>(SDL_NS_PER_MS);
  state->renderer->RenderFrame(frameTimeMs, currentTickSeconds, camera);

  const uint64_t perfCounterEnd = SDL_GetPerformanceCounter();
  state->previousFrameTimeNs = static_cast<uint64_t>(
    static_cast<double>(perfCounterEnd - perfCounterStart) /
    SDL_GetPerformanceFrequency() * SDL_NS_PER_SECOND
  );

  state->previousTickNs = currentTickNs;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  AppState* state = static_cast<AppState*>(appstate);

  ImGui_ImplSDL3_ProcessEvent(event);

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

  if (event->type == SDL_EVENT_KEY_DOWN &&
      !ImGui::GetIO().WantCaptureKeyboard) {
    switch (event->key.scancode) {
      case SDL_SCANCODE_ESCAPE:
        SDL_Log("Escape key pressed, quitting.");
        return SDL_APP_SUCCESS;
      default:
        break;
    }
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  AppState* state = static_cast<AppState*>(appstate);

  SDL_Log("Exiting with result: %d", result);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(state->glContext);
  SDL_DestroyWindow(state->window);

  delete state;
  SDL_Quit();
}
