#pragma once
#include <SDL2/SDL.h>


class App {
  public:
    SDL_Window * window = nullptr;
    SDL_Surface * screenSurface = nullptr;

    App();
    ~App();
    int execute();
    void printError(const char * msg, bool include_sdl_error = true);
    int initialize();
};
