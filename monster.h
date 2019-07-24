#pragma once
#include <SDL2/SDL.h>
#include <list>
#include "entity.h"
#include "utilities.h"
#include <string>
#include "projectile.h"
#include "firePatternParser.h"


const int NUM_SPAWN_FRAMES = 50;

struct MonsterTypeData {
  std::string name;
  std::string monster_img_file;
  int num_frames; // number of frames in monster's usual animation
  int num_death_frames;
  int num_fadeout_frames; // number of frames (from tail end of death frames) during which a fadeout is happening
  vecI monster_img_frame_size; // pixel size of one frame in monster usual animation
  int width;// pixel width and height of monster rendered on screen
  int height;
  SDL_Rect hitbox; // hitbox in LOCAL coords (origin is top left of monster), using IMAGE FILE PXLS
  // (they will be automatically converted to screen pxl units later on)
  vecI projectile_launch_center; // center from which U/D/L/R projectiles will emanate (again in local image file pxls)
  int  projectile_launch_dist;   // distance from projectile_launch_center that U/D/L/R projectiles spawn (again in local image file pxls)
  ProjectileList * bulletManager; // needs to be valid while associated monsters exist. NOT freed by monster.
  std::string firePatternStr;
  int alive_time_per_frame; // global frames per usual animation frame
  int death_time_per_frame; // global frames per death animation frame
};


class Monster {
  public:
    Monster(int x, int y);
    ~Monster();

    int frame = 0;
    int x;
    int y;
    int spawnFrames = 0; // counts how many frames of spawning animation have played. when it reaches
                          // NUM_SPAWN_FRAMES then the monster is "spawned" and active in its normal state
    bool dying = false;
    bool erase_this_monster = false;
    SDL_Rect rect; //rect in which monster is drawn (global coords)
    SDL_Rect hitbox; // monster's hitbox (global coords)
    unsigned int firePatternStepIndex = 0; //index of current step in firing pattern loop
    int firePatternStepCountdown = 0; // countdown for when to move to next step of firing pattern loop
};


class MonsterList :  public EntityManager {
  public:
    MonsterList(MonsterTypeData);
    ~MonsterList();
    virtual int loadMedia(SDL_Renderer *, Logger *);
    virtual void unloadMedia();
    virtual void handleEvent(SDL_Event *);
    virtual void update(App *);
    virtual void render(App *, SDL_Renderer *);

    void createMonster(int x, int y);
    void fireBullet(Monster * monster, DirectionUDLR dir, int speed);

    MonsterTypeData monsterTypeData;
    FirePattern firePattern;

    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Rect * frameToSpriteRect = nullptr;
    SDL_Rect * frameToDeathSpriteRect = nullptr;


    // State
    std::list<Monster*> monsters;

    //Misc
    double screenpx_per_imgpx_x=0;
    double screenpx_per_imgpx_y=0;



};
