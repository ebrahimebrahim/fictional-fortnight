#pragma once
#include <SDL2/SDL.h>

enum ThingyState { // State of the thing controlled by keyboard
  THINGY_UP,
  THINGY_DOWN,
  THINGY_LEFT,
  THINGY_RIGHT,
  THINGY_NEUTRAL,
  THINGY_NUM_STATES, // Count-- must be last
};



class App {
  public:

    // General SDL stuff
    SDL_Window * window = nullptr;
    SDL_Surface * screenSurface = nullptr;

    // Assets
    SDL_Surface * thingyImages [THINGY_NUM_STATES];

    // State flags
    bool quit = false;
    ThingyState thingyState = THINGY_NEUTRAL;

    App();
    ~App();
    void printError(const char * msg, bool include_sdl_error = true);

    int initialize();
    int loadMedia(); // load all media
    void unloadMedia();
    int loadImage(const char *); // load a single image

    int execute();

    void handleEvents();
    void mainLoop();
    void render();

    void handleKeypress(SDL_KeyboardEvent *);

};
