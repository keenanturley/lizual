#define SDL_MAIN_USE_CALLBACKS

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>
#include <stb_image.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>

#include "Camera.h"
#include "Shader.h"

namespace {
constexpr int kDefaultWindowWidth = 640;
constexpr int kDefaultWindowHeight = 480;
const std::filesystem::path kAssetsDir = LIZUAL_ASSETS_DIR;
const std::filesystem::path kVertexShaderPath =
  kAssetsDir / "shaders/default.vert";
const std::filesystem::path kFragmentShaderPath =
  kAssetsDir / "shaders/default.frag";
const std::filesystem::path kContainerTexturePath =
  kAssetsDir / "textures/container.jpg";
const std::filesystem::path kAwesomeFaceTexturePath =
  kAssetsDir / "textures/awesomeface.png";

// clang-format off
// Vertices for a cube
constexpr float kVertices[] = {
  // positions          // texcoords
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

constexpr glm::vec3 kCubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};
// clang-format on
}  // namespace

struct AppState {
  SDL_Window* window;
  // Previous tick (Nanoseconds since SDL was initialized) that was processed by
  // the frameloop
  uint64_t previousTickNs;
  uint64_t previousFrameTimeNs;
  SDL_GLContext glContext;
  Shader* shader;
  std::unique_ptr<Camera> camera;
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

  // Enable blending so I can test transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable Depth testing so we don't get behind fragments drawn in front
  glEnable(GL_DEPTH_TEST);

  // Set up imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  static const std::filesystem::path iniPath =
    std::filesystem::path(SDL_GetBasePath()) / "imgui.ini";
  static const std::filesystem::path logPath =
    std::filesystem::path(SDL_GetBasePath()) / "imgui.log";
  io.IniFilename = iniPath.c_str();
  io.LogFilename = logPath.c_str();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForOpenGL(window, glContext);
  ImGui_ImplOpenGL3_Init();

  // Create a vertex array object (VAO) for the rectangle
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a vertex buffer object (VBO) for the rectangle
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);

  // Create the shader
  // remember to have a try catch block for handling file read exceptions
  Shader* shader;
  try {
    shader = new Shader(kVertexShaderPath, kFragmentShaderPath);
  } catch (const std::ifstream::failure& e) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "Failed to read shader file: %s", e.what()
    );
    return SDL_APP_FAILURE;
  } catch (const std::runtime_error& e) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "Failed to create shader: %s", e.what()
    );
    return SDL_APP_FAILURE;
  }
  shader->Use();

  // Set the vertex attribute pointers
  int stride = 5 * sizeof(float);
  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(0);
  // texture coords
  glVertexAttribPointer(
    1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))
  );
  glEnableVertexAttribArray(1);

  // Load the container texture
  SDL_Log("Loading container texture");
  int width, height, numChannels;
  unsigned char* data =
    stbi_load(kContainerTexturePath.c_str(), &width, &height, &numChannels, 0);
  if (data == nullptr) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_APPLICATION,
      "[stb_image] Failed to load texture from path %s",
      kContainerTexturePath.c_str()
    );
    return SDL_APP_FAILURE;
  }
  SDL_Log("  Loaded container texture: %d x %d", width, height);

  // Create an OpenGL texture
  GLuint textureId;
  glGenTextures(1, &textureId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
  );
  glGenerateMipmap(GL_TEXTURE_2D);
  shader->SetInt("uTexture", 0);

  // Free the image
  stbi_image_free(data);

  // Load the awesome face texture
  SDL_Log("Loading awesome face texture");
  // Flip vertically because it's inversed by default
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data2 = stbi_load(
    kAwesomeFaceTexturePath.c_str(), &width, &height, &numChannels, 0
  );
  stbi_set_flip_vertically_on_load(false);
  if (data == nullptr) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_APPLICATION,
      "[stb_image] Failed to load texture from path %s",
      kContainerTexturePath.c_str()
    );
    return SDL_APP_FAILURE;
  }
  SDL_Log("  Loaded awesome face texture: %d x %d", width, height);

  // Create an OpenGL texture
  GLuint textureId2;
  glGenTextures(1, &textureId2);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureId2);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    width,
    height,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    data2
  );
  glGenerateMipmap(GL_TEXTURE_2D);
  shader->SetInt("uTexture2", 1);

  // Free the image
  stbi_image_free(data2);

  // learnopengl/textures/exercises/4: use a uniform to mix
  // Initialize the mix uniform
  shader->SetFloat("uMix", 0.2f);

  // Configure Camera
  std::unique_ptr camera =
    std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

  uint64_t lastTick = SDL_GetTicksNS();
  *appstate = new AppState{
    window,
    lastTick,
    static_cast<uint64_t>(0),
    glContext,
    shader,
    std::move(camera)
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

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // Show a simple window that we create ourselves. We use a Begin/End pair
  // to create a named window.
  {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin(
      "FPS",
      nullptr,
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs
    );  // Create a window called "Hello, world!" and append into it.

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text(
      "%.1f fps @ %.3f ms/f",
      io.Framerate,
      state->previousFrameTimeNs / static_cast<float>(SDL_NS_PER_MS)
    );

    ImGui::End();
  }

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

  // Time is seconds since the start of the program
  state->shader->SetFloat("uTime", currentTickSeconds);

  // Create Model-View-Projection (MVP) matrices
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = state->camera->GetViewMatrix();

  int windowWidth, windowHeight;
  SDL_GetWindowSizeInPixels(state->window, &windowWidth, &windowHeight);
  float windowAspectRatio = (float)windowWidth / windowHeight;
  glm::mat4 projection =
    glm::perspective(glm::radians(45.0f), windowAspectRatio, 0.1f, 100.0f);

  state->shader->SetUniformMatrix4fv("uView", view);
  state->shader->SetUniformMatrix4fv("uProjection", projection);

  // -- Render
  glClearColor(0.75f, 0.75f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Draw a bunch of cubes
  uint32_t numCubes = sizeof(kCubePositions) / sizeof(kCubePositions[0]);
  for (uint32_t i = 0; i < numCubes; i++) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, kCubePositions[i]);
    float angle = 20.0f * i;
    model = glm::rotate(
      model,
      glm::radians(angle + (currentTickSeconds * 50.0f)),
      glm::vec3(1.0f, 0.3f, 0.5f)
    );
    state->shader->SetUniformMatrix4fv("uModel", model);

    glDrawArrays(GL_TRIANGLES, 0, sizeof(kVertices) / sizeof(kVertices[0]));
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SDL_GL_SwapWindow(state->window);

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

  delete state->shader;

  SDL_Log("Exiting with result: %d", result);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DestroyContext(state->glContext);
  SDL_DestroyWindow(state->window);

  delete state;
  SDL_Quit();
}
