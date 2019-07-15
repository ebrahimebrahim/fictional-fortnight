#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "logger.h"
#include "textbox.h"
#include "playerentity.h"
#include "projectile.h"
#include "utilities.h"


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;



class App {
  public:

    // Sizes of things
    int gamescreen_tiles_x = 25; // tiles in screen area (not including right menu)
    int gamescreen_tiles_y = 20;
    int right_menu_tiles = 7; // number of additional tile-widths to allot to menu
    int tile_width = WINDOW_WIDTH/(gamescreen_tiles_x+right_menu_tiles);
    int tile_height = WINDOW_HEIGHT/gamescreen_tiles_y;
    int right_menu_width = tile_width*right_menu_tiles;
    int gamescreen_width = tile_width*gamescreen_tiles_x;
    int gamescreen_height = tile_height*gamescreen_tiles_y;


    // Misc stuff
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_Joystick * joystick = nullptr;
    Logger log;
    SDL_Color palette [NUM_PALETTE_COLORS];

    // Assets
    TTF_Font * font = nullptr;

    // App state
    bool quit = false;
    Uint32 timerStart = 0;
    Uint32 lastFrameTime = 0;
    int fps = 0;
    Uint32 frame = 0;

    // Game state
    PlayerEntity playerEntity;
    ProjectileList projectileList;

    // Rectangles for viewports
    SDL_Rect screen_rect = {0,0,gamescreen_width,gamescreen_height};
    SDL_Rect right_menu_rect = {gamescreen_width,0,right_menu_width,WINDOW_HEIGHT};

    // Textboxes
    TextBox * peupTextBox = nullptr;
    TextBox * timerTextBox = nullptr;
    TextBox * fpsTextBox = nullptr;



    App();
    ~App();
    void printError(const char * msg, bool include_sdl_error = true);

    int initialize();
    int loadMedia(); // load all media
    void unloadMedia();

    int execute();

    void handleEvents();
    void mainLoop();
    void render();

    void handleKeypress(SDL_KeyboardEvent *);

};
