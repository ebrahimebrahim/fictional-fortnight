#include "playerentity.h"
#include "utilities.h"
#include "main.h"


PlayerEntity::PlayerEntity() {

}


PlayerEntity::~PlayerEntity() {

}


int PlayerEntity::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage("thingy.png",renderer,log);
	if (sprites==nullptr){
		log->error("Error: Some media was not loaded.");
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


void PlayerEntity::handleKeypress(SDL_KeyboardEvent * key){
  switch (key->keysym.scancode) {
    case SDL_SCANCODE_LEFT:
      orientation = PLAYERENTITY_DIRECTION_LEFT;
      tryMove = PLAYERENTITY_DIRECTION_LEFT;
      break;
    case SDL_SCANCODE_RIGHT:
      orientation = PLAYERENTITY_DIRECTION_RIGHT;
      tryMove = PLAYERENTITY_DIRECTION_RIGHT;
      break;
    case SDL_SCANCODE_UP:
      orientation = PLAYERENTITY_DIRECTION_UP;
      tryMove = PLAYERENTITY_DIRECTION_UP;
      break;
    case SDL_SCANCODE_DOWN:
      orientation = PLAYERENTITY_DIRECTION_DOWN;
      tryMove = PLAYERENTITY_DIRECTION_DOWN;
      break;
    default:
      break;
  }
}

void PlayerEntity::update(App * app){
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
}
