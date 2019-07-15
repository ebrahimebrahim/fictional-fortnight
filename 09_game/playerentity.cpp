#include "playerentity.h"
#include "utilities.h"
#include "main.h"


PlayerEntity::PlayerEntity() {

}


PlayerEntity::~PlayerEntity() {

}


int PlayerEntity::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage("player.png",renderer,log);
	if (sprites==nullptr){
		log->error("Error: PlayerEntity sprite was not loaded.");
		return -1;
	}
	orientationToSpriteRect[PLAYERENTITY_DIRECTION_NEUTRAL]={0,0,5,5};
	orientationToSpriteRect[PLAYERENTITY_DIRECTION_UP]={5,0,5,5};
	orientationToSpriteRect[PLAYERENTITY_DIRECTION_DOWN]={10,0,5,5};
	orientationToSpriteRect[PLAYERENTITY_DIRECTION_LEFT]={15,0,5,5};
	orientationToSpriteRect[PLAYERENTITY_DIRECTION_RIGHT]={20,0,5,5};

  return 0;
}


void PlayerEntity::unloadMedia(){
  SDL_DestroyTexture(sprites);
  sprites = nullptr;
}


void PlayerEntity::handleEvent(SDL_Event * event){
  if (event->type==SDL_KEYDOWN) {
    switch (event->key.keysym.scancode) {
      case SDL_SCANCODE_LEFT: tryLeft(); break;
      case SDL_SCANCODE_RIGHT: tryRight(); break;
      case SDL_SCANCODE_UP: tryUp(); break;
      case SDL_SCANCODE_DOWN: tryDown(); break;
      case SDL_SCANCODE_SPACE: tryShoot = true;
      default: break;
    }
  }
  else if (event->type==SDL_JOYHATMOTION) {
    switch (event->jhat.value) {
      case SDL_HAT_LEFT: tryLeft(); break;
      case SDL_HAT_RIGHT: tryRight(); break;
      case SDL_HAT_UP: tryUp(); break;
      case SDL_HAT_DOWN: tryDown(); break;
      default: break;
    }
  }
}

void PlayerEntity::update(App * app) {
  if (tryMove!=PLAYERENTITY_DIRECTION_NEUTRAL){
		switch (tryMove) {
			case PLAYERENTITY_DIRECTION_UP:
				if (y > 0) y--;
				break;
			case PLAYERENTITY_DIRECTION_DOWN:
				if (y < app->gamescreen_tiles_y-1) y++;
				break;
			case PLAYERENTITY_DIRECTION_LEFT:
				if (x > 0) x--;
				break;
			case PLAYERENTITY_DIRECTION_RIGHT:
				if (x < app->gamescreen_tiles_x-1) x++;
				break;
			default: break;
		}
		tryMove=PLAYERENTITY_DIRECTION_NEUTRAL;
	}

  if (tryShoot) {
      ProjectileDirection dir = PROJECTILE_DIRECTION_UP;
      switch (orientation) {
        case PLAYERENTITY_DIRECTION_UP: dir = PROJECTILE_DIRECTION_UP; break;
        case PLAYERENTITY_DIRECTION_DOWN: dir = PROJECTILE_DIRECTION_DOWN; break;
        case PLAYERENTITY_DIRECTION_LEFT: dir = PROJECTILE_DIRECTION_LEFT; break;
        case PLAYERENTITY_DIRECTION_RIGHT: dir = PROJECTILE_DIRECTION_RIGHT; break;
        default: break;
      }
      app->projectileList.createProjectile(x*app->tile_width,y*app->tile_height,2,dir);
      tryShoot = false;
  }
}

void PlayerEntity::render(App * app, SDL_Renderer * renderer){
  SDL_Rect target_rect = {x*app->tile_width,y*app->tile_height,app->tile_width,app->tile_height};
  SDL_RenderCopy(renderer, sprites, &(orientationToSpriteRect[orientation]), &target_rect);
}

void PlayerEntity::tryLeft() {
  orientation = PLAYERENTITY_DIRECTION_LEFT;
  tryMove = PLAYERENTITY_DIRECTION_LEFT;
}

void PlayerEntity::tryRight() {
  orientation = PLAYERENTITY_DIRECTION_RIGHT;
  tryMove = PLAYERENTITY_DIRECTION_RIGHT;
}

void PlayerEntity::tryUp() {
  orientation = PLAYERENTITY_DIRECTION_UP;
  tryMove = PLAYERENTITY_DIRECTION_UP;
}

void PlayerEntity::tryDown() {
  orientation = PLAYERENTITY_DIRECTION_DOWN;
  tryMove = PLAYERENTITY_DIRECTION_DOWN;
}
