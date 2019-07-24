#include "projectile.h"
#include "utilities.h"
#include "main.h"


extern Globals globals;

Projectile::Projectile(int x, int y, int v, DirectionUDLR dir) : x(x), y(y), v(v), dir(dir) {
}

Projectile::~Projectile() {

}



ProjectileList::ProjectileList(ProjectileTypeData projectileTypeData) : projectileTypeData(projectileTypeData) {

  // Convert detonation points from image file pixel units to screen pixel units
  double screenpx_per_imgpx_proj_x = double(projectileTypeData.width) / double(projectileTypeData.projectile_img_frame_size.x);
  double screenpx_per_imgpx_proj_y = double(projectileTypeData.height) / double(projectileTypeData.projectile_img_frame_size.y);
  double screenpx_per_imgpx_expl_x = double(projectileTypeData.explosion_width) / double(projectileTypeData.explosion_img_frame_size.x);
  double screenpx_per_imgpx_expl_y = double(projectileTypeData.explosion_height) / double(projectileTypeData.explosion_img_frame_size.y);
  this->projectileTypeData.projectile_detonation_point.x *= screenpx_per_imgpx_proj_x;
  this->projectileTypeData.projectile_detonation_point.y *= screenpx_per_imgpx_proj_y;
  this->projectileTypeData.explosion_detonation_point.x  *= screenpx_per_imgpx_expl_x;
  this->projectileTypeData.explosion_detonation_point.y  *= screenpx_per_imgpx_expl_y;
  this->projectileTypeData.projectile_hitbox.x *= screenpx_per_imgpx_proj_x;
  this->projectileTypeData.projectile_hitbox.w *= screenpx_per_imgpx_proj_x;
  this->projectileTypeData.projectile_hitbox.y *= screenpx_per_imgpx_proj_y;
  this->projectileTypeData.projectile_hitbox.h *= screenpx_per_imgpx_proj_y;
  this->projectileTypeData.explosion_hitbox.x  *= screenpx_per_imgpx_expl_x;
  this->projectileTypeData.explosion_hitbox.w  *= screenpx_per_imgpx_expl_x;
  this->projectileTypeData.explosion_hitbox.y  *= screenpx_per_imgpx_expl_y;
  this->projectileTypeData.explosion_hitbox.h  *= screenpx_per_imgpx_expl_y;
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

      // update projectile hitbox, which needs to be correct for other stuff to work
      updateProjectileHitbox(projectile);

      // check if it should start exploding and align explosion and set projectile->hitbox to be correct.
      if (app->rectContents(projectile->hitbox, projectile) & (CONTAINS_OBSTRUCTION | CONTAINS_DEADLY_EXPLOSION | CONTAINS_PROJECTILE)) {
        projectile->exploding = true;
        projectile->animation_frame_countdown = projectileTypeData.explosion_time_per_frame;

        vecI r1(projectileTypeData.width/2,projectileTypeData.height/2);
        vecI r2(projectileTypeData.explosion_width/2,projectileTypeData.explosion_height/2);
        vecI newCoords = rotatePoint( projectileTypeData.projectile_detonation_point - projectileTypeData.explosion_detonation_point + r2 - r1,
                                      vecI(0,0),projectile->dir)
                         + (r1-r2) + vecI(projectile->x,projectile->y);
        projectile->x = newCoords.x;
        projectile->y = newCoords.y;

        updateProjectileHitbox(projectile);
      }
    }
    else {
      if (projectile->animation_frame_countdown == 0) {
        ++(projectile->explode_frame);
        projectile->animation_frame_countdown = projectileTypeData.explosion_time_per_frame-1;
      }
      else --projectile->animation_frame_countdown;
      --projectile->explosion_global_frames_remaining;
      if (projectile->explode_frame >= projectileTypeData.num_explosion_frames) projectile->erase_this_projectile=true;
    }

  }

  // Erase any dead projectiles:
  for (std::list<Projectile*>::iterator iter = projectiles.begin(); iter != projectiles.end(); ++iter)
    if ((*iter)->erase_this_projectile) {
      delete (*iter);
      iter = projectiles.erase(iter);
    }
}

void ProjectileList::render(App * app, SDL_Renderer * renderer) {
  for (Projectile * projectile : projectiles) {
    if (!projectile->exploding) {
      SDL_Rect target_rect = {projectile->x,projectile->y,projectileTypeData.width,projectileTypeData.height};
      SDL_RenderCopyEx(renderer, sprites, &(frameToSpriteRect[projectile->frame]), &target_rect,
                       globals.directionToRotAngle[projectile->dir], nullptr, SDL_FLIP_NONE);
    }
    else {
      SDL_Rect target_rect = {projectile->x,projectile->y,projectileTypeData.explosion_width,projectileTypeData.explosion_height};
      int alphaMod = std::min( (255 * projectile->explosion_global_frames_remaining) /
                               (projectileTypeData.num_fadeout_frames * projectileTypeData.explosion_time_per_frame) ,
                              255);
      SDL_SetTextureAlphaMod(explosionFrames,alphaMod);
      SDL_RenderCopyEx(renderer, explosionFrames, &(frameToExplosionRect[projectile->explode_frame]),&(target_rect),
                       globals.directionToRotAngle[projectile->dir], nullptr, SDL_FLIP_NONE);
      SDL_SetTextureAlphaMod(sprites,255);
    }
    // SDL_SetRenderDrawColor(renderer, 255,0,0,255);  //TEST
    // SDL_RenderDrawRect(renderer, &(projectile->hitbox)); // TEST
    // SDL_SetRenderDrawColor(renderer, 0,255,255,255);  //TEST
    // SDL_RenderDrawPoint(renderer,projectile->x,projectile->y); // TEST
  }
}

void ProjectileList::updateProjectileHitbox(Projectile * projectile) {
  const SDL_Rect & local_hitbox = projectile->exploding ?
                                  projectileTypeData.explosion_hitbox :
                                  projectileTypeData.projectile_hitbox;
  projectile->hitbox = local_hitbox;
  projectile->hitbox.x += projectile->x;
  projectile->hitbox.y += projectile->y;

  vecI center = vecI(projectile->x,projectile->y) + // top left of projectile/explosion
                ( projectile->exploding
                  ? vecI(projectileTypeData.explosion_width/2,projectileTypeData.explosion_height/2)
                  : vecI(projectileTypeData.width/2,projectileTypeData.height/2) ); // center of rotation
  projectile->hitbox = rotateRect(projectile->hitbox,center,projectile->dir);
}

void ProjectileList::createProjectile(int x, int y, int v, DirectionUDLR dir) {
  Projectile * new_projectile = new Projectile(x,y,v,dir);
  updateProjectileHitbox(new_projectile);
  new_projectile->explosion_global_frames_remaining = projectileTypeData.explosion_time_per_frame * projectileTypeData.num_explosion_frames;
  projectiles.push_front(new_projectile);
}
