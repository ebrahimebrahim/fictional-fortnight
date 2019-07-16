#pragma once
#include <SDL2/SDL.h>
#include <list>
#include "entity.h"
#include "utilities.h"

const int NUM_PROJECTILE_FRAMES = 2;
const int NUM_EXPLOSION_FRAMES = 6;
const int MAX_NUM_PROJECTILES = 200;


class Projectile {
  public:
    Projectile(int x, int y, int v, DirectionUDLR dir);
    ~Projectile();

    int frame = 0;
    int x;
    int y;
    int v;
    DirectionUDLR dir;
    bool exploding = false;
    int explode_frame = 0;
    bool erase_this_projectile = false;
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

    void createProjectile(int x, int y, int v, DirectionUDLR dir);


    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Texture * explosionFrames = nullptr;
    SDL_Rect frameToSpriteRect [NUM_PROJECTILE_FRAMES];
    SDL_Rect frameToExplosionRect [NUM_EXPLOSION_FRAMES];
    double projectileDirectionToRotAngle [DIRECTION_NUM_STATES];

    // State
    std::list<Projectile*> projectiles;

    //Constants, for now
    int width = 18;
    int height = 40;
    int explosion_width = 60;
    int explosion_height = 60;


};
