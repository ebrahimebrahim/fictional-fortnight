#include "playerentity.h"
#include "utilities.h"
#include "main.h"
#include <algorithm>


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
  if (event->type==SDL_KEYDOWN && !(event->key.repeat)) {
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

  const Uint8 * keyState = SDL_GetKeyboardState(nullptr);

  if (!keyState[SDL_SCANCODE_LEFT] && !keyState[SDL_SCANCODE_RIGHT] && !keyState[SDL_SCANCODE_UP] && !keyState[SDL_SCANCODE_DOWN])
    tryMove = PLAYERENTITY_DIRECTION_NEUTRAL;

  if (tryMove!=PLAYERENTITY_DIRECTION_NEUTRAL){
		switch (tryMove) {
			case PLAYERENTITY_DIRECTION_UP:
				y = std::max(0,y-v);
				break;
			case PLAYERENTITY_DIRECTION_DOWN:
				y = std::min(app->gamescreen_height-height,y+v);
				break;
			case PLAYERENTITY_DIRECTION_LEFT:
				x = std::max(0,x-v);
				break;
			case PLAYERENTITY_DIRECTION_RIGHT:
				x = std::min(app->gamescreen_width-width,x+v);
				break;
			default: break;
		}
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
      app->projectileList.createProjectile(x,y,v+2,dir);
      tryShoot = false;
  }
}

void PlayerEntity::render(App * app, SDL_Renderer * renderer){
  SDL_Rect target_rect = {x,y,width,height};
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
