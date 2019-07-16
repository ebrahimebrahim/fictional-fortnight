#pragma once
#include <SDL2/SDL.h>
#include <forward_list>
#include "entity.h"
#include "utilities.h"

const int NUM_PROJECTILE_FRAMES = 2;
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
    SDL_Rect frameToSpriteRect [NUM_PROJECTILE_FRAMES];
    double projectileDirectionToRotAngle [DIRECTION_NUM_STATES];

    // State
    std::forward_list<Projectile*> projectiles;

    //Constants, for now
    int width = 18;
    int height = 40;


};
