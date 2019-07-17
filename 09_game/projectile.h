#pragma once
#include <SDL2/SDL.h>
#include <list>
#include "entity.h"
#include "utilities.h"
#include <string>


struct ProjectileTypeData {
  int num_frames;
  int num_explosion_frames;
  int num_deadly_explosion_frames;
  std::string projectile_img_file;
  vecI projectile_img_frame_size;
  std::string explosion_img_file;
  vecI explosion_img_frame_size;
};


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
    SDL_Rect explosionRect;
};


class ProjectileList :  public EntityManager {
  public:
    ProjectileList(ProjectileTypeData);
    ~ProjectileList();
    virtual int loadMedia(SDL_Renderer *,Logger *);
    virtual void unloadMedia();
    virtual void handleEvent(SDL_Event *);
    virtual void update(App *); // Update step to go into main game loop
    virtual void render(App *, SDL_Renderer *);

    void createProjectile(int x, int y, int v, DirectionUDLR dir);

    ProjectileTypeData projectileTypeData;

    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Texture * explosionFrames = nullptr;
    SDL_Rect * frameToSpriteRect = nullptr;
    SDL_Rect * frameToExplosionRect;
    double projectileDirectionToRotAngle [DIRECTION_NUM_STATES];

    // State
    std::list<Projectile*> projectiles;

    //Constants, for now
    int width = 18;
    int height = 40;
    int explosion_width = 60;
    int explosion_height = 60;



};
