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
  global_frames_till_next_death_animation_frame = death_animation_frame_time;
}


PlayerEntity::~PlayerEntity() {

}


int PlayerEntity::loadMedia(SDL_Renderer * renderer, Logger * log){
  sprites = loadImage("player_remake.png",renderer,log);
	if (sprites==nullptr){
		log->error("Error: PlayerEntity sprite was not loaded.");
		return -1;
	}
  for (int i = 0 ; i < NUM_PLAYER_SPRITE_RECTS ; ++i) {
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
    //if (event->key.keysym.scancode == SDL_SCANCODE_LSHIFT) trySpeed = true;  // no more speed boost!
  }

}

void PlayerEntity::update(App * app) {

  // If dying then just keep the death animation ticking and ignore all other usual update steps
  if (hitpoints<=0) {
    if (global_frames_till_next_death_animation_frame > 0)
      --global_frames_till_next_death_animation_frame;
    else {
      global_frames_till_next_death_animation_frame=death_animation_frame_time;
      if (death_animation_frame < num_death_animation_frames-1) ++death_animation_frame;
      else app->lose();
    }
    return;
  }

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

  // If player is in an obstruction, which can happen after orientation change, then pop them out of it.
  playerHitbox = getPlayerHitbox(x,y,orientation);
  if (app->rectContents(playerHitbox,this) & CONTAINS_OBSTRUCTION) {
    bool success=false; int s = 1;
    vecI pos(x,y);
    while (!success && s<9999) {
      vecI posToTry [4] = {pos + vecI(s,0), pos + vecI(0,s), pos + vecI(-s,0), pos + vecI(0,-s)};
      for (int i=0; i<4; ++i) {
        SDL_Rect tryHitbox = getPlayerHitbox(posToTry[i].x,posToTry[i].y,orientation);
        if (!(app->rectContents(tryHitbox,this) & CONTAINS_OBSTRUCTION)) {
          x = posToTry[i].x; y = posToTry[i].y;
          success=true;
          break;
        }
      }
      ++s;
    }
  }


  // Carry out movement to the extent possible
  vecI d = globals.directionToUnitVector[orientation];
  vecI locX = globals.directionToLocalX[orientation];

  if (tryMove!=DIRECTION_NEUTRAL) {
    for (v = 0; v<move_speed; ++v) {
      vecI pos(x,y);
      vecI forwardPos(pos+d);
      SDL_Rect forwardHitbox = getPlayerHitbox(forwardPos.x,forwardPos.y,orientation);
      ContainsBitmask forwardContents = app->rectContents(forwardHitbox,this);
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
      app->projectileList->createProjectile(p.x,p.y,v+3,orientation,true);
      missile_cooldown_countdown=missile_cooldown;
  }
  tryShoot = false;

  if (missile_cooldown_countdown > 0) --missile_cooldown_countdown;


  // check if player should be harmed now:

  if (app->rectContents(playerHitbox) & (CONTAINS_DEADLY_EXPLOSION | CONTAINS_DEADLY_TO_PLAYER))
    if (hit_timer == 0) {
      app->addScore(SCORE_CHANGE_WHEN_PLAYER_HIT);
      hit_timer = hit_cooldown;
      SDL_SetTextureAlphaMod(sprites,128);

      // remove shield if it was engaged. if not, then take hull damage.
      if (shield_timer==0) {
        shield_timer = shield_recharge_time;
      }
      else {
        --hitpoints;
      }
    }

  // tick down timers
  if (hit_timer>0) {
    --hit_timer;
    if (hit_timer==1) SDL_SetTextureAlphaMod(sprites,255);
  }
  if (shield_timer>0) {
    --shield_timer;
  }

}

void PlayerEntity::render(App * app, SDL_Renderer * renderer){

  if (hitpoints >0) {
    // render player
    SDL_Rect * sprite_rect = &(sprite_rects[PLAYER_MAX_HP-hitpoints]);
    SDL_Rect target_rect = getUnrotatedFullRect();
    SDL_Point center = {int(float(player_hitbox_x_img + player_hitbox_width_img/2)*screenpx_per_imgpx),
                        int(float(player_hitbox_y_img + player_hitbox_height_img/2)*screenpx_per_imgpx)};
    SDL_RenderCopyEx(renderer, sprites, sprite_rect, &target_rect,
                     globals.directionToRotAngle[orientation],&center,SDL_FLIP_NONE);

    // render shield
    if (shield_timer==0)
      SDL_RenderCopyEx(renderer, sprites, &(sprite_rects[PLAYER_MAX_HP]), &target_rect,
                       globals.directionToRotAngle[orientation],&center,SDL_FLIP_NONE);
  }
  else {
    // render death animation frame
    SDL_Rect * sprite_rect = &(sprite_rects[PLAYER_MAX_HP+death_animation_frame+1]);
    SDL_Rect target_rect = getUnrotatedFullRect();
    SDL_Point center = {int(float(player_hitbox_x_img + player_hitbox_width_img/2)*screenpx_per_imgpx),
                        int(float(player_hitbox_y_img + player_hitbox_height_img/2)*screenpx_per_imgpx)};
    SDL_RenderCopyEx(renderer, sprites, sprite_rect, &target_rect,
                     globals.directionToRotAngle[orientation],&center,SDL_FLIP_NONE);
  }
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
