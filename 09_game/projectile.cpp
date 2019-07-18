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

      // update project rect, which needs to be correct for other stuff to work
      projectile->rect = {projectile->x,projectile->y,width,height};
      vecI topLeft(projectile->x,projectile->y);
      if (projectile->dir == DIRECTION_LEFT || projectile->dir == DIRECTION_RIGHT) {
        topLeft = topLeft + vecI(width/2,height/2) - vecI(height/2,width/2);
        projectile->rect.x = topLeft.x;
        projectile->rect.y = topLeft.y;
        projectile->rect.w = height;
        projectile->rect.h = width;
      }

      // check if it should start exploding
      if (app->rectContents(projectile->rect, projectile) & (CONTAINS_OBSTRUCTION | CONTAINS_DEADLY_EXPLOSION)) {
        vecI topLeft(projectile->x,projectile->y);
        vecI explosionTopLeft = topLeft + vecI(width/2,height/2) // projectile center
                                - vecI(explosion_width/2,explosion_height/2); // explosion top left
        projectile->x = explosionTopLeft.x;
        projectile->y = explosionTopLeft.y;
        projectile->rect = {projectile->x,projectile->y,explosion_width,explosion_height};
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
      SDL_RenderCopy(renderer, explosionFrames, &(frameToExplosionRect[projectile->explode_frame]),&(projectile->rect));
    }
    SDL_SetRenderDrawColor(renderer, 255,0,0,255);  //TEST
    SDL_RenderDrawRect(renderer, &(projectile->rect)); // TEST
  }
}

void ProjectileList::createProjectile(int x, int y, int v, DirectionUDLR dir) {
  Projectile * new_projectile = new Projectile(x,y,v,dir);
  new_projectile->rect = {x,y,width,height};
  projectiles.push_front(new_projectile);
}
