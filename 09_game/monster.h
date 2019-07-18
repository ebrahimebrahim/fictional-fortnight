#pragma once
#include <SDL2/SDL.h>
#include <list>
#include "entity.h"
#include "utilities.h"
#include <string>
#include "projectile.h"


struct MonsterTypeData {
  std::string name;
  std::string monster_img_file;
  int num_frames; // number of frames in monster's usual animation
  int num_death_frames;
  vecI monster_img_frame_size; // pixel size of one frame in monster usual animation
  int width;// pixel width and height of monster rendered on screen
  int height;
  SDL_Rect hitbox; // hitbox in LOCAL coords (origin is top left of monster)
  ProjectileList * bulletManager; // needs to be valid while associated monsters exist. NOT freed by monster.
  int bullet_speed; // pixels per frame
};


class Monster {
  public:
    Monster(int x, int y);
    ~Monster();

    int frame = 0;
    int x;
    int y;
    bool dying = false;
    bool erase_this_monster = false;
    SDL_Rect rect; //rect in which monster is drawn (global coords)
    SDL_Rect hitbox; // monster's hitbox (global coords)
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

    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Rect * frameToSpriteRect = nullptr;
    SDL_Rect * frameToDeathSpriteRect = nullptr;


    // State
    std::list<Monster*> monsters;




};
