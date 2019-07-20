#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "logger.h"
#include "textbox.h"
#include "playerentity.h"
#include "projectile.h"
#include "monster.h"
#include "utilities.h"
#include <vector>


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int SCORE_FOR_KILLING_MONSTER = 5;
const int SCORE_CHANGE_WHEN_PLAYER_HIT = -10;
const int SCORE_TO_LOSE = -50;
const int SCORE_PER_LEVEL_ADVANCE = 200; // The score to win is this times the number of levels.
                                         // The number of levels is just the number of monster types
                                         // Possibly +1 if a final boss is made


class App {
  public:


    // Sizes of things
    int right_menu_width = WINDOW_WIDTH * 0.2;
    int gamescreen_width = WINDOW_WIDTH - right_menu_width;
    int gamescreen_height = WINDOW_HEIGHT;


    // Misc stuff
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    Logger log;
    SDL_Color palette [NUM_PALETTE_COLORS];
    int num_levels = 0; //set in app initialization

    // Assets
    TTF_Font * font = nullptr;

    // App state
    bool quit = false;
    Uint32 timerStart = 0;
    Uint32 lastFrameTime = 0;
    int fps = 0;
    Uint32 frame = 0;

    // Game state
    int score = 0;
    int level = 1; // The indices from 0 to level are the indices of entityManagers_monster that can be spawned
    bool won  = false;
    bool lost = false;
    std::vector<EntityManager *> entityManagers_nonprojectile;
    std::vector<ProjectileList *> entityManagers_projectile;
    std::vector<MonsterList *> entityManagers_monster;
    PlayerEntity * playerEntity = nullptr;
    ProjectileList * projectileList = nullptr;
    MonsterList * monster1List = nullptr;
    ProjectileList * monster1bulletList = nullptr;

    // Rectangles for viewports
    SDL_Rect screen_rect = {0,0,gamescreen_width,gamescreen_height};
    SDL_Rect right_menu_rect = {gamescreen_width,0,right_menu_width,WINDOW_HEIGHT};

    // Textboxes
    TextBox * scoreTextBox = nullptr;
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


    // Methods that will be used by the entity managers
    ContainsBitmask rectContents(const SDL_Rect & r, const void * ignore = nullptr);
    void addScore(int);
    void spawnMonster();

};