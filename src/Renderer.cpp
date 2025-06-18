#include "Renderer.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>
#include <SDL3/SDL_log.h>

#include <filesystem>
#include <fstream>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

namespace {
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

std::unique_ptr<Renderer> Renderer::Create(
  SDL_Window* window, SDL_GLContext glContext
) {
  SDL_Log("Creating Renderer");

  // Set the default viewport size
  int widthInPixels;
  int heightInPixels;
  if (!SDL_GetWindowSizeInPixels(window, &widthInPixels, &heightInPixels)) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR,
      "SDL_GetWindowSizeInPixels failed: %s",
      SDL_GetError()
    );
    return nullptr;
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
  std::unique_ptr<Shader> shader;
  try {
    shader = std::make_unique<Shader>(kVertexShaderPath, kFragmentShaderPath);
  } catch (const std::ifstream::failure& e) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "Failed to read shader file: %s", e.what()
    );
    return nullptr;
  } catch (const std::runtime_error& e) {
    SDL_LogCritical(
      SDL_LOG_CATEGORY_ERROR, "Failed to create shader: %s", e.what()
    );
    return nullptr;
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

  // Load textures first before binding to ensure loader doesn't trample
  // bindings of glActiveTexture Load the container texture
  std::unique_ptr<Texture> containerTexture =
    Texture::Load(kContainerTexturePath);
  std::unique_ptr<Texture> awesomeFaceTexture =
    Texture::Load(kAwesomeFaceTexturePath);

  // Now bind after loading
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, containerTexture->textureId);
  shader->SetInt("uTexture", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, awesomeFaceTexture->textureId);
  shader->SetInt("uTexture2", 1);

  // learnopengl/textures/exercises/4: use a uniform to mix
  // Initialize the mix uniform
  shader->SetFloat("uMix", 0.2f);

  return std::unique_ptr<Renderer>(
    new Renderer(window, glContext, std::move(shader))
  );
}

void Renderer::RenderFrame(
  float frameTimeMs, float currentTickSeconds, Camera& camera
) {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(0, 0));
  ImGui::Begin(
    "FPS",
    nullptr,
    ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoInputs
  );

  ImGuiIO& io = ImGui::GetIO();
  ImGui::Text("%.1f fps @ %.3f ms/f", io.Framerate, frameTimeMs);
  ImGui::End();

  // Time is seconds since the start of the program
  shader_->SetFloat("uTime", currentTickSeconds);

  // Create Model-View-Projection (MVP) matrices
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = camera.GetViewMatrix();

  int windowWidth, windowHeight;
  SDL_GetWindowSizeInPixels(window_, &windowWidth, &windowHeight);
  float windowAspectRatio = (float)windowWidth / windowHeight;
  glm::mat4 projection =
    glm::perspective(glm::radians(45.0f), windowAspectRatio, 0.1f, 100.0f);

  shader_->SetUniformMatrix4fv("uView", view);
  shader_->SetUniformMatrix4fv("uProjection", projection);

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
    shader_->SetUniformMatrix4fv("uModel", model);

    glDrawArrays(GL_TRIANGLES, 0, sizeof(kVertices) / sizeof(kVertices[0]));
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SDL_GL_SwapWindow(window_);
}
