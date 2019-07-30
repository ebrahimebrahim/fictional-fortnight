#include "playerentity.h"
#include "utilities.h"
#include "main.h"
#include <algorithm>
#include <vector>

extern Globals globals;

SDL_Rect PlayerEntity::getPlayerHitbox(int x1, int y1, DirectionUDLR dir) {
  SDL_Rect hitbox = {x1 - int(float(player_hitbox_width_img)/2 * screenpx_per_imgpx),
                     y1 - int(float(player_hitbox_height_img)/2 * screenpx_per_imgpx),
                     int(float(player_hitbox_width_img)*screenpx_per_imgpx),
                     int(float(player_hitbox_height_img)*screenpx_per_imgpx)};
  return rotateRect(hitbox,vecI(x1,y1),dir);
}

SDL_Rect PlayerEntity::getUnrotatedFullRect() {
  SDL_Rect rect = {x - int((float(player_hitbox_width_img)/2  + player_hitbox_x_img) * screenpx_per_imgpx),
                   y - int((float(player_hitbox_height_img)/2 + player_hitbox_y_img) * screenpx_per_imgpx),
                   int(float(img_width)*screenpx_per_imgpx),
                   int(float(img_height)*screenpx_per_imgpx)};
  return rect;
}

PlayerEntity::PlayerEntity() : lastDirectionalKeys(128,SDL_SCANCODE_UNKNOWN) {
  screenpx_per_imgpx = float(player_hitbox_width_screen) / float(player_hitbox_width_img);
  playerHitbox = getPlayerHitbox(x,y,orientation);
}


PlayerEntity::~PlayerEntity() {

}


int PlayerEntity::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage("player_remake.png",renderer,log);
	if (sprites==nullptr){
		log->error("Error: PlayerEntity sprite was not loaded.");
		return -1;
	}
  for (int i = 0 ; i < num_player_sprite_rects ; ++i) {
    sprite_rects[i]={img_width*i,0,img_width,img_height};
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
    if (event->key.keysym.scancode == SDL_SCANCODE_SPACE)  tryShoot = true;
    if (event->key.keysym.scancode == SDL_SCANCODE_LSHIFT) trySpeed = true;
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
      SDL_Rect forwardHitbox = getPlayerHitbox(forwardPos.x,forwardPos.y,orientation);
      ContainsBitmask forwardContents = app->rectContents(SDL_Rect(forwardHitbox),this);
      if (!(forwardContents & CONTAINS_OBSTRUCTION)) {
        x = forwardPos.x; y =forwardPos.y;
        playerHitbox = forwardHitbox; // should optimize to a move I guess? IDK how to "move"
      }
      else break;
    }

	}


  // resolve shoot attempt
  if (tryShoot && missile_cooldown_countdown==0) {

      vecI p = vecI(x,y)
               + globals.directionToUnitVector[orientation]*((int(float(player_hitbox_height_img)*screenpx_per_imgpx) + app->projectileList->projectileTypeData.height)/2)
               - vecI(app->projectileList->projectileTypeData.width/2,app->projectileList->projectileTypeData.height/2);
      app->projectileList->createProjectile(p.x,p.y,v+3,orientation);
      missile_cooldown_countdown=missile_cooldown;
  }
  tryShoot = false;

  if (missile_cooldown_countdown > 0) --missile_cooldown_countdown;

  // resolve spped attempt
  if (trySpeed && speedboost_cooldown_countdown==0) {
    move_speed += speedboost;
    speedboost_cooldown_countdown = speedboost_cooldown;
    speedboost_time_left = speedboost_duration;
  }
  if (speedboost_cooldown_countdown > 0) --speedboost_cooldown_countdown;
  if (speedboost_time_left > 0) {
    --speedboost_time_left;
    if (speedboost_time_left == 0) {
      move_speed -= speedboost;
    }
  }
  trySpeed = false;

  // check if player should be harmed now:

  if (app->rectContents(playerHitbox) & (CONTAINS_DEADLY_EXPLOSION | CONTAINS_DEADLY_TO_PLAYER))
    if (hit_cooldown == 0) {
      app->addScore(SCORE_CHANGE_WHEN_PLAYER_HIT);
      hit_cooldown = 60;
      SDL_SetTextureAlphaMod(sprites,60);
    }

  if (hit_cooldown>0) {
    --hit_cooldown;
    if (hit_cooldown==1) SDL_SetTextureAlphaMod(sprites,255);
  }


}

void PlayerEntity::render(App * app, SDL_Renderer * renderer){
  int sprite_rect_index = 0 ;
  SDL_Rect target_rect = getUnrotatedFullRect();
  SDL_Point center = {int(float(player_hitbox_x_img + player_hitbox_width_img/2)*screenpx_per_imgpx),
                      int(float(player_hitbox_y_img + player_hitbox_height_img/2)*screenpx_per_imgpx)};
  SDL_RenderCopyEx(renderer, sprites, &(sprite_rects[sprite_rect_index]), &target_rect,
                   globals.directionToRotAngle[orientation],&center,SDL_FLIP_NONE);
  // SDL_SetRenderDrawColor(renderer, 0,255,0,255);  //TEST
  // SDL_RenderDrawRect(renderer, &(target_rect)); // TEST
  // SDL_SetRenderDrawColor(renderer, 255,0,0,255);  //TEST
  // SDL_RenderDrawRect(renderer, &(playerHitbox)); // TEST
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
