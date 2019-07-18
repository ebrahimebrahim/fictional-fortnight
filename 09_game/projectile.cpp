#include "projectile.h"
#include "utilities.h"
#include "main.h"


extern Globals globals;

Projectile::Projectile(int x, int y, int v, DirectionUDLR dir) : x(x), y(y), v(v), dir(dir) {

}

Projectile::~Projectile() {

}



ProjectileList::ProjectileList(ProjectileTypeData projectileTypeData) : projectileTypeData(projectileTypeData) {

}


ProjectileList::~ProjectileList() {
  for (Projectile * projectile : projectiles) delete projectile;
}


int ProjectileList::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage(projectileTypeData.projectile_img_file.c_str(),renderer,log);
	if (sprites==nullptr){
		log->error("Error: A projectile sprite was not loaded.");
		return -1;
	}

  explosionFrames = loadImage(projectileTypeData.explosion_img_file.c_str(),renderer,log);
	if (explosionFrames==nullptr){
		log->error("Error: An explosion animation was not loaded.");
		return -1;
	}

  frameToSpriteRect = new SDL_Rect[projectileTypeData.num_frames];
  for (int i = 0; i < projectileTypeData.num_frames; i++) {
    frameToSpriteRect[i] = {projectileTypeData.projectile_img_frame_size.x*i,0,
                            projectileTypeData.projectile_img_frame_size.x  , projectileTypeData.projectile_img_frame_size.y};
  }

  frameToExplosionRect = new SDL_Rect[projectileTypeData.num_explosion_frames];
  for (int i = 0; i < projectileTypeData.num_explosion_frames; i++) {
    frameToExplosionRect[i] = {projectileTypeData.explosion_img_frame_size.x*i,0,
                               projectileTypeData.explosion_img_frame_size.x  , projectileTypeData.explosion_img_frame_size.y};
  }

  return 0;
}


void ProjectileList::unloadMedia(){
  SDL_DestroyTexture(sprites); sprites = nullptr;
  SDL_DestroyTexture(explosionFrames); explosionFrames = nullptr;
  delete frameToSpriteRect; frameToSpriteRect = nullptr;
  delete frameToExplosionRect; frameToExplosionRect = nullptr;
}


void ProjectileList::handleEvent(SDL_Event * event){

}


void ProjectileList::update(App * app){
  for (Projectile * projectile : projectiles) {

    if (!projectile->exploding) {
      // increment animation frame
      if (app->frame % 10 == 0) projectile->frame = (projectile->frame + 1) % projectileTypeData.num_frames;

      // move projectile
      switch (projectile->dir) {
        case DIRECTION_UP: projectile->y -= projectile->v; break;
        case DIRECTION_DOWN: projectile->y += projectile->v; break;
        case DIRECTION_LEFT: projectile->x -= projectile->v; break;
        case DIRECTION_RIGHT: projectile->x += projectile->v; break;
        default: break;
      }

      // check if it should start exploding
      vecI topLeft(projectile->x,projectile->y);
      SDL_Rect projectile_rect = {projectile->x,projectile->y,width,height};
      if (projectile->dir == DIRECTION_LEFT || projectile->dir == DIRECTION_RIGHT) {
        topLeft = topLeft + vecI(width/2,height/2) - vecI(height/2,width/2);
        projectile_rect.x = topLeft.x;
        projectile_rect.y = topLeft.y;
        projectile_rect.w = height;
        projectile_rect.h = width;
      }
      if (app->containsObstruction(projectile_rect)) {
        vecI topLeft(projectile->x,projectile->y);
        vecI explosionTopLeft = topLeft + vecI(width/2,height/2) // projectile center
                                - vecI(explosion_width/2,explosion_height/2); // explosion top left
        projectile->x = explosionTopLeft.x;
        projectile->y = explosionTopLeft.y;
        projectile->exploding = true;
      }
    }
    else {
      if (app->frame % 5 == 0) ++(projectile->explode_frame);
      if (projectile->explode_frame >= projectileTypeData.num_explosion_frames) projectile->erase_this_projectile=true;
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
      SDL_RenderCopyEx(renderer, sprites, &(frameToSpriteRect[projectile->frame]), &target_rect,
                       globals.directionToRotAngle[projectile->dir], nullptr, SDL_FLIP_NONE);
    }
    else {
      projectile->explosionRect = {projectile->x,projectile->y,explosion_width,explosion_height};
      SDL_RenderCopy(renderer, explosionFrames, &(frameToExplosionRect[projectile->explode_frame]),&(projectile->explosionRect));
    }
  }
}

void ProjectileList::createProjectile(int x, int y, int v, DirectionUDLR dir) {
  projectiles.push_front(new Projectile(x,y,v,dir));
}
