#include "playerentity.h"
#include "utilities.h"
#include "main.h"
#include <algorithm>

extern Globals globals;

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
	orientationToSpriteRect[DIRECTION_UP]={0,0,5,5};
	orientationToSpriteRect[DIRECTION_DOWN]={5,0,5,5};
	orientationToSpriteRect[DIRECTION_LEFT]={10,0,5,5};
	orientationToSpriteRect[DIRECTION_RIGHT]={15,0,5,5};

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
    tryMove = DIRECTION_NEUTRAL;
  else {
    for (SDL_Scancode sc : lastDirectionalKeys) {
      if (keyState[sc]) {
        orientation = directionalKeyToPlayerDirection(sc);
        tryMove = directionalKeyToPlayerDirection(sc);
        break;
      }
    }
  }

  if (tryMove!=DIRECTION_NEUTRAL){
		switch (tryMove) {
			case DIRECTION_UP:
				y = std::max(0,y-v);
				break;
			case DIRECTION_DOWN:
				y = std::min(app->gamescreen_height-height,y+v);
				break;
			case DIRECTION_LEFT:
				x = std::max(0,x-v);
				break;
			case DIRECTION_RIGHT:
				x = std::min(app->gamescreen_width-width,x+v);
				break;
			default: break;
		}
	}

  if (tryShoot) {

      vecI p = vecI(x,y) + vecI(width/2,height/2)
               + globals.directionToUnitVector[orientation]*((width + app->projectileList->height)/2)
               - vecI(app->projectileList->width/2,app->projectileList->height/2);
      app->projectileList->createProjectile(p.x,p.y,v+2,orientation);
      tryShoot = false;
  }
}

void PlayerEntity::render(App * app, SDL_Renderer * renderer){
  SDL_Rect target_rect = {x,y,width,height};
  SDL_RenderCopy(renderer, sprites, &(orientationToSpriteRect[orientation]), &target_rect);
}

DirectionUDLR PlayerEntity::directionalKeyToPlayerDirection(SDL_Scancode sc) {
  switch(sc) {
    case SDL_SCANCODE_LEFT: return DIRECTION_LEFT;
    case SDL_SCANCODE_RIGHT: return DIRECTION_RIGHT;
    case SDL_SCANCODE_UP: return DIRECTION_UP;
    case SDL_SCANCODE_DOWN: return DIRECTION_DOWN;
    default: return DIRECTION_NEUTRAL;
  }
}
