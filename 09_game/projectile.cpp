#include "projectile.h"
#include "utilities.h"
#include "main.h"


Projectile::Projectile(int x, int y, int v, DirectionUDLR dir) : x(x), y(y), v(v), dir(dir) {

}

Projectile::~Projectile() {

}



ProjectileList::ProjectileList() {
  projectileDirectionToRotAngle[DIRECTION_NEUTRAL] = 0;
  projectileDirectionToRotAngle[DIRECTION_UP] = 0;
  projectileDirectionToRotAngle[DIRECTION_RIGHT] = 90;
  projectileDirectionToRotAngle[DIRECTION_DOWN] = 180;
  projectileDirectionToRotAngle[DIRECTION_LEFT] = 270;
}


ProjectileList::~ProjectileList() {
  for (Projectile * projectile : projectiles) delete projectile;
}


int ProjectileList::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage("projectile.png",renderer,log);
	if (sprites==nullptr){
		log->error("Error: Projectile sprite was not loaded.");
		return -1;
	}
	frameToSpriteRect[0]={0,0,17,39};
	frameToSpriteRect[1]={17,0,17,39};

  return 0;
}


void ProjectileList::unloadMedia(){
  SDL_DestroyTexture(sprites);
  sprites = nullptr;
}


void ProjectileList::handleEvent(SDL_Event * event){

}


void ProjectileList::update(App * app){
  for (Projectile * projectile : projectiles) {

    // increment animation frame
    if (app->frame % 10 == 0) projectile->frame = (projectile->frame + 1) % NUM_PROJECTILE_FRAMES;

    // move projectile
    switch (projectile->dir) {
      case DIRECTION_UP: projectile->y -= projectile->v; break;
      case DIRECTION_DOWN: projectile->y += projectile->v; break;
      case DIRECTION_LEFT: projectile->x -= projectile->v; break;
      case DIRECTION_RIGHT: projectile->x += projectile->v; break;
      default: break;
    }

    // // check if it should start exploding
    // if (projectile->x )


  }
}

void ProjectileList::render(App * app, SDL_Renderer * renderer) {
  for (Projectile * projectile : projectiles) {
    SDL_Rect target_rect = {projectile->x,projectile->y,width,height};
    SDL_RenderCopyEx(renderer, sprites, &(frameToSpriteRect[projectile->frame]), &target_rect, projectileDirectionToRotAngle[projectile->dir], nullptr, SDL_FLIP_NONE);
  }
}

void ProjectileList::createProjectile(int x, int y, int v, DirectionUDLR dir) {
  projectiles.push_front(new Projectile(x,y,v,dir));
}
