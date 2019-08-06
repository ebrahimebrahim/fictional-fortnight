#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <list>
#include "entity.h"
#include "utilities.h"
#include <string>


struct ProjectileTypeData {
  int id; // a hopefully unique identifier for this projectile type
  int num_frames;
  int num_explosion_frames;
  int num_deadly_explosion_frames; // number of frames (from head end of explosion frames) during which explosion hitbox is active
  int num_fadeout_frames; // number of frames (from tail end of explosion frames) during which a fadeout is happening
  SDL_Rect projectile_hitbox; // projectile hitbox in LOCAL coords IMAGE FILE PXLS (origin is top left of upward-facing projectile)
  SDL_Rect explosion_hitbox;  // explosion hitbox in LOCAL coords IMAGE FILE PXLS (origin is top left of upward-facing projectile)
  // These will automatically get converted from the originally given pixel units.
  std::string projectile_img_file;
  std::string projectile_launch_sound_file;
  vecI projectile_img_frame_size;
  std::string explosion_img_file;
  std::string projectile_die_sound_file;
  vecI explosion_img_frame_size;
  int width;
  int height;
  int explosion_width;
  int explosion_height;
  vecI explosion_detonation_point;  // point on rendered explosion image (LOCAL coords) from which detonation starts
  // From main you should pass this in IMAGE PIXELS, so that you can just measure in graphics editor.
  // It will be later converted to screen pixels
  vecI projectile_detonation_point; // point on rendered projectile image (LOCAL coords) from which detonation starts
  // Also pass this one in IMAGE pixels
  // (the above two points are going to match at the moment of detonation)
  int explosion_time_per_frame; // global frames per explosion frame
  bool explosion_only_harms_player; // as opposed to also detonating projectiles and harming other monsters
};


class Projectile {
  public:
    Projectile(int x, int y, int v, DirectionUDLR dir);
    ~Projectile();

    int frame = 0;
    int explosion_global_frames_remaining = 0; // this counts game frames to do cool stuff with explode animation
    int x;
    int y;
    int v;
    DirectionUDLR dir;
    bool launched_by_player = false; // turned on when projectile is launched by player
    bool exploding = false;
    int explode_frame = 0;
    int animation_frame_countdown = 0; // countdown to next frame of explosion animation
    bool erase_this_projectile = false;
    SDL_Rect hitbox; // hitbox of explosion or of projectile, depending on whether exploding is turned on
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

    void updateProjectileHitbox(Projectile *);
    void createProjectile(int x, int y, int v, DirectionUDLR dir, bool created_by_player=false);

    ProjectileTypeData projectileTypeData;

    //Assets
    SDL_Texture * sprites = nullptr;
    SDL_Texture * explosionFrames = nullptr;
    SDL_Rect * frameToSpriteRect = nullptr;
    SDL_Rect * frameToExplosionRect;
    Mix_Chunk * launch_sound_chunk = nullptr;
    Mix_Chunk * die_sound_chunk = nullptr;

    // State
    std::list<Projectile*> projectiles;



};
