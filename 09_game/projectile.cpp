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

  explosionFrames = loadImage("explode.png",renderer,log);
	if (explosionFrames==nullptr){
		log->error("Error: Explosion animation was not loaded.");
		return -1;
	}

	frameToSpriteRect[0]={0,0,17,39};
	frameToSpriteRect[1]={17,0,17,39};

  for (int i = 0; i < NUM_EXPLOSION_FRAMES; i++) {
    frameToExplosionRect[i] = {40*i,0,40,40};
  }

  return 0;
}


void ProjectileList::unloadMedia(){
  SDL_DestroyTexture(sprites);
  sprites = nullptr;
  SDL_DestroyTexture(explosionFrames);
  explosionFrames = nullptr;
}


void ProjectileList::handleEvent(SDL_Event * event){

}


void ProjectileList::update(App * app){
  for (Projectile * projectile : projectiles) {

    if (!projectile->exploding) {
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

      // check if it should start exploding
      vecI front = vecI(projectile->x,projectile->y) + vecI(width/2,height/2) + (height/2)*directionToUnitVector(projectile->dir);
      int fx = front.x; int fy = front.y;
      if (fx < 0 || fx > app->gamescreen_width || fy < 0 || fy > app->gamescreen_height)
        projectile->exploding = true;
    }
    else {
      if (app->frame % 5 == 0) ++(projectile->explode_frame);
      if (projectile->explode_frame >= NUM_EXPLOSION_FRAMES) projectile->erase_this_projectile=true;
    }

  }

  // Erase any dead projectiles:
  for (std::list<Projectile*>::iterator iter = projectiles.begin(); iter != projectiles.end(); ++iter)
    if ((*iter)->erase_this_projectile)
      iter = projectiles.erase(iter);
}

void ProjectileList::render(App * app, SDL_Renderer * renderer) {
  for (Projectile * projectile : projectiles) {
    if (!projectile->exploding) {
      SDL_Rect target_rect = {projectile->x,projectile->y,width,height};
      SDL_RenderCopyEx(renderer, sprites, &(frameToSpriteRect[projectile->frame]), &target_rect, projectileDirectionToRotAngle[projectile->dir], nullptr, SDL_FLIP_NONE);
    }
    else {
      vecI explodeCenter = vecI(projectile->x,projectile->y) + vecI(width/2,height/2) + (height/4)*directionToUnitVector(projectile->dir);
      vecI explodeTopLeft = explodeCenter - vecI(explosion_width/2,explosion_height/2);
      SDL_Rect target_rect = {explodeTopLeft.x,explodeTopLeft.y,explosion_width,explosion_height};
      SDL_RenderCopy(renderer, explosionFrames, &(frameToExplosionRect[projectile->explode_frame]),&target_rect);
    }
  }
}

void ProjectileList::createProjectile(int x, int y, int v, DirectionUDLR dir) {
  projectiles.push_front(new Projectile(x,y,v,dir));
}
