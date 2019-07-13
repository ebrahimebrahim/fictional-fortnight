#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "logger.h"
#include "textbox.h"



const int NUM_TILES_X = 25; // tiles in screen area (not including right menu)
const int NUM_TILES_Y = 20;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float RIGHT_MENU_TILES = 7; // number of additional tile-widths to allot to menu
const int TILE_WIDTH = WINDOW_WIDTH/(NUM_TILES_X+RIGHT_MENU_TILES);
const int TILE_HEIGHT = WINDOW_HEIGHT/NUM_TILES_Y;

const int RIGHT_MENU_WIDTH = TILE_WIDTH*RIGHT_MENU_TILES;
const int SCREEN_WIDTH = TILE_WIDTH*NUM_TILES_X;
const int SCREEN_HEIGHT = TILE_HEIGHT*NUM_TILES_Y;



enum PaletteColor {
  PALETTE_BLACK,
  PALETTE_WHITE,
  NUM_PALETTE_COLORS // Count-- must be last
};

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

    // General stuff
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_Joystick * joystick = nullptr;
    Logger log;
    SDL_Color palette [NUM_PALETTE_COLORS];

    // Assets
    SDL_Texture * thingySprites = nullptr;
    TTF_Font * font = nullptr;

    // Game state
    bool quit = false;
    ThingyState thingyState = THINGY_NEUTRAL;
    TryMoveState tryMoveState = TRY_MOVE_STATE_NOT_TRYING;
    SDL_Point pos = {NUM_TILES_X/2,NUM_TILES_Y/2};
    Uint32 timerStart = 0;

    // Rectangles for viewports and sprites
    SDL_Rect screen_rect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_Rect right_menu_rect = {SCREEN_WIDTH,0,RIGHT_MENU_WIDTH,WINDOW_HEIGHT};
    SDL_Rect thingyStateToSpriteRect [THINGY_NUM_STATES];

    // Textboxes
    TextBox * peupTextBox = nullptr;
    TextBox * timerTextBox = nullptr;



    App();
    ~App();
    void printError(const char * msg, bool include_sdl_error = true);

    int initialize();
    int loadMedia(); // load all media
    void unloadMedia();
    SDL_Texture * loadImage(const char * filename); // load a single image, returning nullptr on fail

    int execute();

    void handleEvents();
    void mainLoop();
    void render();

    void handleKeypress(SDL_KeyboardEvent *);
    void handleJoyhat(SDL_JoyHatEvent * jhat);

};
