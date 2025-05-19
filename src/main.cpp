#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

#include <memory>

struct AppState {
  SDL_Window *window;
  SDL_Renderer *renderer;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s",
                    SDL_GetError());
    SDL_Quit();
    return SDL_APP_FAILURE;
  }
  SDL_Log("SDL initialized successfully");

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  if (!SDL_CreateWindowAndRenderer("Lizual", 640, 480, 0, &window, &renderer)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
                    "SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
    SDL_Quit();
    return SDL_APP_FAILURE;
  }
  AppState *state = new AppState{window, renderer};
  *appstate = state;
  SDL_Log("Window and renderer created successfully");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);

  SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(state->renderer);
  SDL_RenderPresent(state->renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  AppState *state = static_cast<AppState *>(appstate);

  if (event->type == SDL_EVENT_QUIT) {
    SDL_Log("Received quit event");
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  AppState *state = static_cast<AppState *>(appstate);

  SDL_Log("Exiting with result: %d", result);
  SDL_DestroyRenderer(state->renderer);
  SDL_DestroyWindow(state->window);
  SDL_Quit();
}