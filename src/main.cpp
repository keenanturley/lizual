#define SDL_MAIN_USE_CALLBACKS

#include <glad/gl.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>

#include <memory>

struct AppState {
  SDL_Window *window;
  SDL_GLContext glContext;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "SDL_Init failed: %s",
                    SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_Log("SDL initialized successfully");

  SDL_Window *window = SDL_CreateWindow(
      "Lizual", 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
                    "SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (glContext == nullptr) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "SDL_GL_CreateContext failed: %s",
                    SDL_GetError());
    return SDL_APP_FAILURE;
  }

  int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  int major = GLAD_VERSION_MAJOR(version);
  int minor = GLAD_VERSION_MINOR(version);

  SDL_Log("GLAD version: %d.%d", major, minor);

  *appstate = new AppState{
      window,
      glContext,
  };
  SDL_Log("App initialization complete");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *state = static_cast<AppState *>(appstate);

  glClearColor(0.75f, 0.75f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(state->window);

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
  SDL_GL_DestroyContext(state->glContext);
  SDL_DestroyWindow(state->window);

  delete state;
  SDL_Quit();
}