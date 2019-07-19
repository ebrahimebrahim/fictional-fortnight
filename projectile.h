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
  int width;
  int height;
  int explosion_width;
  int explosion_height;
  int explosion_time_per_frame; // global frames per explosion frame
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
    int animation_frame_countdown = 0; // countdown to next frame of explosion animation
    bool erase_this_projectile = false;
    SDL_Rect rect; // rect of explosion or of projectile, depending on whether exploding is turned on
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

    void updateProjectileRect(Projectile *);
    void createProjectile(int x, int y, int v, DirectionUDLR dir);

    ProjectileTypeData projectileTypeData;

    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Texture * explosionFrames = nullptr;
    SDL_Rect * frameToSpriteRect = nullptr;
    SDL_Rect * frameToExplosionRect;

    // State
    std::list<Projectile*> projectiles;



};
