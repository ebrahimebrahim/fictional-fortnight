#pragma once
#include <SDL2/SDL.h>
#include "logger.h"


class App; // Forward declaration


enum PlayerEntityDirection { // possible orientation and movement directions
  PLAYERENTITY_DIRECTION_UP,
  PLAYERENTITY_DIRECTION_DOWN,
  PLAYERENTITY_DIRECTION_LEFT,
  PLAYERENTITY_DIRECTION_RIGHT,
  PLAYERENTITY_DIRECTION_NEUTRAL,
  PLAYERENTITY_DIRECTION_NUM_STATES // Count-- must be last
};


class PlayerEntity {
  public:
    PlayerEntity();
    ~PlayerEntity();
    int loadMedia(SDL_Renderer *,Logger *);
    void unloadMedia();

    void handleKeypress(SDL_KeyboardEvent *);
    void update(App *); // Update step to go into main game loop

    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Rect orientationToSpriteRect [PLAYERENTITY_DIRECTION_NUM_STATES];

    // State
    PlayerEntityDirection orientation = PLAYERENTITY_DIRECTION_NEUTRAL;
    PlayerEntityDirection tryMove = PLAYERENTITY_DIRECTION_NEUTRAL;
    int x = 0;
    int y = 0;


};
