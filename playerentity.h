#pragma once
#include <SDL2/SDL.h>
#include "entity.h"
#include "utilities.h"


const int NUM_PLAYER_SPRITE_RECTS = 10;
const int PLAYER_MAX_HP = 3; // This is also the number of rects in the player spritesheet which show different hull dmgs



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
    Mix_Chunk * hull_damage_sound;
    Mix_Chunk * death_sound;
    Mix_Chunk * shield_down_sound;
    Mix_Chunk * shield_up_sound;
    Mix_Chunk * reloaded_sound;

    // Useful tables and rects
    SDL_Rect sprite_rects [NUM_PLAYER_SPRITE_RECTS];
    DirectionUDLR directionalKeyToPlayerDirection(SDL_Scancode);

    // State
    DirectionUDLR orientation = DIRECTION_RIGHT;
    DirectionUDLR tryMove = DIRECTION_NEUTRAL;
    CircleBuffer<SDL_Scancode> lastDirectionalKeys;
    bool tryShoot = false;
    int x = 100; // x,y on game screen of center of the player hitbox
    int y = 100;
    int v = 0; // current actual speed
    int move_speed = 5; // current move speed if tried to move and not obstructed (can be boosted)
    SDL_Rect playerHitbox;
    int missile_cooldown_countdown = 0;
    int hit_timer = 0; // counts down invincibility after getting hit
    int shield_timer = 0; // If 0 then the shield is on. when shield lost resets to shield_recharge_time
    int death_animation_frame = 0;
    int global_frames_till_next_death_animation_frame;
    int hitpoints = PLAYER_MAX_HP; // for hull damage. when 0 the player is currently dying and death animation is playing.

    // Constants (for now)
    int img_width = 200;  // image width and height in pixels of a single sprite in the image of sprites
    int img_height = 200;
    int player_hitbox_x_img=49; // top left corner of player hitbox in local sprite image coords in pixels
    int player_hitbox_y_img=49;
    int player_hitbox_width_img = 102; // width and height og player hitbox in local sprite img coords in pxls
    int player_hitbox_height_img = 82;
    int player_hitbox_width_screen = 20; // width of player on screen. This relative to player_hitbox_width_img sets the scale.
    int missile_cooldown = 60;
    int hit_cooldown = 50; // invincibility time after getting hit
    int shield_recharge_time = 200;
    int num_death_animation_frames = 6;
    int death_animation_frame_time = 10;

    float screenpx_per_imgpx=0; // deduced from player_hitbox_width_img and player_hitbox_width_screen


};
