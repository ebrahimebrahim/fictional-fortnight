#include "playerentity.h"
#include "utilities.h"
#include "main.h"
#include <algorithm>


PlayerEntity::PlayerEntity() : lastDirectionalKeys(128,SDL_SCANCODE_UNKNOWN) {

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
    if (isArrowKey(event->key.keysym.scancode)) lastDirectionalKeys.push(event->key.keysym.scancode);
    if (event->key.keysym.scancode == SDL_SCANCODE_SPACE) tryShoot = true;
  }

}

void PlayerEntity::update(App * app) {

  const Uint8 * keyState = SDL_GetKeyboardState(nullptr);

  if (!keyState[SDL_SCANCODE_LEFT] && !keyState[SDL_SCANCODE_RIGHT] && !keyState[SDL_SCANCODE_UP] && !keyState[SDL_SCANCODE_DOWN])
    tryMove = PLAYERENTITY_DIRECTION_NEUTRAL;
  else {
    for (SDL_Scancode sc : lastDirectionalKeys) {
      if (keyState[sc]) {
        orientation = directionalKeyToPlayerDirection(sc);
        tryMove = directionalKeyToPlayerDirection(sc);
        break;
      }
    }
  }

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

PlayerEntityDirection PlayerEntity::directionalKeyToPlayerDirection(SDL_Scancode sc) {
  switch(sc) {
    case SDL_SCANCODE_LEFT: return PLAYERENTITY_DIRECTION_LEFT;
    case SDL_SCANCODE_RIGHT: return PLAYERENTITY_DIRECTION_RIGHT;
    case SDL_SCANCODE_UP: return PLAYERENTITY_DIRECTION_UP;
    case SDL_SCANCODE_DOWN: return PLAYERENTITY_DIRECTION_DOWN;
    default: return PLAYERENTITY_DIRECTION_NEUTRAL;
  }
}
