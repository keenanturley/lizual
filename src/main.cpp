#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }
  std::cout << "SDL Initialized successfully!" << std::endl;
  return 0;
}