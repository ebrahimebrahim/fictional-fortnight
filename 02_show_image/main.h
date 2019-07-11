#pragma once
#include <SDL2/SDL.h>


class App {
  public:

    // General SDL stuff
    SDL_Window * window = nullptr;
    SDL_Surface * screenSurface = nullptr;

    // Assets
    SDL_Surface * peupImage = nullptr;

    // State flags
    bool quit = false;

    App();
    ~App();
    void printError(const char * msg, bool include_sdl_error = true);

    int initialize();
    int loadMedia();
    void unloadMedia();

    int execute();

    void handleEvents();
    void mainLoop();
    void render();

};
