#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "Camera.h"
#include "Shader.h"

class Renderer {
 public:
  // Factory method to create a renderer, which may fail and return nullptr.
  static std::unique_ptr<Renderer> Create(
    SDL_Window* window, SDL_GLContext glContext
  );

  // Remove copy and assignment constructors
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  // Renders a frame.
  void RenderFrame(float frameTimeMs, float currentTickSeconds, Camera& camera);

 private:
  // Private constructor to use after data is gathered from the factory method.
  Renderer(
    SDL_Window* window,
    SDL_GLContext glContext,
    std::unique_ptr<Shader>&& shader
  )
      : window_(window), glContext_(glContext), shader_(std::move(shader)) {};

  SDL_Window* window_;
  SDL_GLContext glContext_;
  std::unique_ptr<Shader> shader_;
};
