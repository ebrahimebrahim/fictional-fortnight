#pragma once
#include <SDL2/SDL.h>
#include "entity.h"
#include "utilities.h"


const int num_player_sprite_rects = 4;



class PlayerEntity :  public EntityManager {
  public:
    PlayerEntity();
    ~PlayerEntity();
    virtual int loadMedia(SDL_Renderer *,Logger *);
    virtual void unloadMedia();
    virtual void handleEvent(SDL_Event *);
    virtual void update(App *); // Update step to go into main game loop
    virtual void render(App *, SDL_Renderer *);

    SDL_Rect getPlayerHitbox(int,int,DirectionUDLR); // get player hitbox from given  x and y position and rotation
    SDL_Rect getUnrotatedFullRect(); // get pre-rotation sprite (not hitbox) rect in game screen

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
    int x = 100; // x,y on game screen of center of the player hitbox
    int y = 100;
    int v = 0; // current actual speed
    int move_speed = 5; // current move speed if tried to move and not obstructed (can be boosted)
    SDL_Rect playerHitbox;
    int missile_cooldown_countdown = 0;
    int speedboost_cooldown_countdown = 0; // time left before speedbost can be used again
    int speedboost_time_left = 0; // time (updates) left to remain in boosted state
    int hit_cooldown = 0;

    // Constants (for now)
    int img_width = 200;  // image width and height in pixels of a single sprite in the image of sprites
    int img_height = 200;
    int player_hitbox_x_img=50; // top left corner of player hitbox in local sprite image coords in pixels
    int player_hitbox_y_img=50;
    int player_hitbox_width_img = 100; // width and height og player hitbox in local sprite img coords in pxls
    int player_hitbox_height_img = 80;
    int player_hitbox_width_screen = 20; // width of player on screen. This relative to player_hitbox_width_img sets the scale.
    int missile_cooldown = 60;
    int speedboost_cooldown = 300;
    int speedboost_duration = 100; // time to remain in boosted state
    int speedboost = 5; // amount to boost speed when speed boost is used

    float screenpx_per_imgpx=0; // deduced from player_hitbox_width_img and player_hitbox_width_screen


};
