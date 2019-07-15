#pragma once
#include <SDL2/SDL.h>
#include <forward_list>
#include "entity.h"

const int NUM_PROJECTILE_FRAMES = 2;
const int MAX_NUM_PROJECTILES = 200;

enum ProjectileDirection { // possible orientation and movement directions
  PROJECTILE_DIRECTION_UP,
  PROJECTILE_DIRECTION_DOWN,
  PROJECTILE_DIRECTION_LEFT,
  PROJECTILE_DIRECTION_RIGHT,
  PROJECTILE_DIRECTION_NUM_STATES // Count-- must be last
};


class Projectile {
  public:
    Projectile(int x, int y, int v, ProjectileDirection dir);
    ~Projectile();

    int frame = 0;
    int x;
    int y;
    int v;
    ProjectileDirection dir;
};


class ProjectileList :  public Entity {
  public:
    ProjectileList();
    ~ProjectileList();
    virtual int loadMedia(SDL_Renderer *,Logger *);
    virtual void unloadMedia();
    virtual void handleEvent(SDL_Event *);
    virtual void update(App *); // Update step to go into main game loop
    virtual void render(App *, SDL_Renderer *);

    void createProjectile(int x, int y, int v, ProjectileDirection dir);


    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Rect frameToSpriteRect [NUM_PROJECTILE_FRAMES];
    double projectileDirectionToRotAngle [PROJECTILE_DIRECTION_NUM_STATES];

    // State
    std::forward_list<Projectile*> projectiles;


};
