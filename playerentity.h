#pragma once
#include <SDL2/SDL.h>
#include "entity.h"
#include "utilities.h"


const int num_player_sprite_rects = 5;



class PlayerEntity :  public EntityManager {
  public:
    PlayerEntity();
    ~PlayerEntity();
    virtual int loadMedia(SDL_Renderer *,Logger *);
    virtual void unloadMedia();
    virtual void handleEvent(SDL_Event *);
    virtual void update(App *); // Update step to go into main game loop
    virtual void render(App *, SDL_Renderer *);

    // Assets
    SDL_Texture * sprites = nullptr;

    // Useful tables and rects
    SDL_Rect sprite_rects [num_player_sprite_rects];
    DirectionUDLR directionalKeyToPlayerDirection(SDL_Scancode);

    // State
    DirectionUDLR orientation = DIRECTION_RIGHT;
    DirectionUDLR tryMove = DIRECTION_NEUTRAL;
    CircleBuffer<SDL_Scancode> lastDirectionalKeys;
    bool tryShoot = false;
    bool trySpeed = false;
    int x = 100; // x,y of top left corner of the player entity
    int y = 100;
    int v = 0; // current actual speed
    int move_speed = 5; // current move speed if tried to move and not obstructed (can be boosted)
    SDL_Rect playerRect;
    int missile_cooldown_countdown = 0;
    int speedboost_cooldown_countdown = 0; // time left before speedbost can be used again
    int speedboost_time_left = 0; // time (updates) left to remain in boosted state
    int hit_cooldown = 0;

    // Constants (for now)
    int width = 20;
    int height = 20;
    int missile_cooldown = 60;
    int speedboost_cooldown = 300;
    int speedboost_duration = 100; // time to remain in boosted state
    int speedboost = 5; // amount to boost speed when speed boost is used


};
