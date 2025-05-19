#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

int main(int argc, char *argv[]) {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s",
                    SDL_GetError());
    SDL_Quit();
    return 1;
  }
  SDL_Log("SDL initialized successfully");

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (!SDL_CreateWindowAndRenderer("Lizual", 640, 480, 0, &window, &renderer)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
                    "SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  SDL_Log("Window and renderer created successfully");

  SDL_Event event;
  while (1) {
    SDL_PollEvent(&event);
    if (event.type == SDL_EVENT_QUIT) {
      break;
    }
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }

  SDL_Log("Exiting...");
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}