#pragma once
#include <SDL2/SDL.h>



const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int NUM_TILES_X = 20;
const int NUM_TILES_Y = 20;

const int TILE_WIDTH = SCREEN_WIDTH/NUM_TILES_X;
const int TILE_HEIGHT = SCREEN_HEIGHT/NUM_TILES_Y;


enum ThingyState { // State of the thing controlled by keyboard
  THINGY_UP,
  THINGY_DOWN,
  THINGY_LEFT,
  THINGY_RIGHT,
  THINGY_NEUTRAL,
  THINGY_NUM_STATES // Count-- must be last
};

enum TryMoveState {
  TRY_MOVE_STATE_NOT_TRYING,
  TRY_MOVE_STATE_UP,
  TRY_MOVE_STATE_DOWN,
  TRY_MOVE_STATE_LEFT,
  TRY_MOVE_STATE_RIGHT,
  TRY_MOVE_NUM_STATES // Count-- must be last
};



class App {
  public:

    // General SDL stuff
    SDL_Window * window = nullptr;
    SDL_Surface * screenSurface = nullptr;

    // Assets
    SDL_Surface * thingyImages [THINGY_NUM_STATES];

    // State
    bool quit = false;
    ThingyState thingyState = THINGY_NEUTRAL;
    TryMoveState tryMoveState = TRY_MOVE_STATE_NOT_TRYING;
    SDL_Point pos = {NUM_TILES_X/2,NUM_TILES_Y/2};



    App();
    ~App();
    void printError(const char * msg, bool include_sdl_error = true);

    int initialize();
    int loadMedia(); // load all media
    void unloadMedia();
    SDL_Surface * loadImage(const char * filename); // load a single image, returning nullptr on fail

    int execute();

    void handleEvents();
    void mainLoop();
    void render();

    void handleKeypress(SDL_KeyboardEvent *);

};
