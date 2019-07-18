#include "playerentity.h"
#include "utilities.h"
#include "main.h"
#include <algorithm>
#include <vector>

extern Globals globals;

PlayerEntity::PlayerEntity() : lastDirectionalKeys(128,SDL_SCANCODE_UNKNOWN) {
  playerRect = {x,y,width,height};
}


PlayerEntity::~PlayerEntity() {

}


int PlayerEntity::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage("player.png",renderer,log);
	if (sprites==nullptr){
		log->error("Error: PlayerEntity sprite was not loaded.");
		return -1;
	}
  for (int i = 0 ; i < num_player_sprite_rects ; ++i) {
    sprite_rects[i]={7*i,0,7,7};
  }

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

  if (!keyState[SDL_SCANCODE_LEFT] && !keyState[SDL_SCANCODE_RIGHT] && !keyState[SDL_SCANCODE_UP] && !keyState[SDL_SCANCODE_DOWN]) {
    tryMove = DIRECTION_NEUTRAL;
    v=0;
  }
  else {
    for (SDL_Scancode sc : lastDirectionalKeys) {
      if (keyState[sc]) {
        orientation = directionalKeyToPlayerDirection(sc);
        tryMove = directionalKeyToPlayerDirection(sc);
        break;
      }
    }
  }


  vecI d = globals.directionToUnitVector[orientation];
  vecI locX = globals.directionToLocalX[orientation];

  if (tryMove!=DIRECTION_NEUTRAL) {
    for (v = 0; v<move_speed; ++v) {
      vecI pos(x,y);
      vecI forwardPos(pos+d);
      SDL_Rect forwardRect = {forwardPos.x,forwardPos.y,width,height};
      if (!(app->rectContents(SDL_Rect(forwardRect),this) & CONTAINS_OBSTRUCTION)) {
        x = forwardPos.x; y =forwardPos.y;
        playerRect = forwardRect; // should optimize to a move I guess? IDK how to "move"
      }
      else break;
    }

	}


  // resolve shoot attempt
  if (tryShoot && missile_cooldown_countdown==0) {

      vecI p = vecI(x,y) + vecI(width/2,height/2)
               + globals.directionToUnitVector[orientation]*((width + app->projectileList->height)/2)
               - vecI(app->projectileList->width/2,app->projectileList->height/2);
      app->projectileList->createProjectile(p.x,p.y,v+3,orientation);
      missile_cooldown_countdown=missile_cooldown;
  }
  tryShoot = false;

  if (missile_cooldown_countdown > 0) --missile_cooldown_countdown;


  // check if player should die now:

  if (app->rectContents(playerRect) & CONTAINS_DEADLY_EXPLOSION)
    app->peupTextBox->updateText("D:");

}

void PlayerEntity::render(App * app, SDL_Renderer * renderer){
  int sprite_rect_index = (missile_cooldown - missile_cooldown_countdown) * 3 / missile_cooldown ;
  SDL_RenderCopyEx(renderer, sprites, &(sprite_rects[sprite_rect_index]), &playerRect,
                   globals.directionToRotAngle[orientation],nullptr,SDL_FLIP_NONE);
  SDL_SetRenderDrawColor(renderer, 255,0,0,255);  //TEST
  SDL_RenderDrawRect(renderer, &(playerRect)); // TEST
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
