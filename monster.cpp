#include "monster.h"
#include "utilities.h"
#include "main.h"
#include <cstdlib>
#include <algorithm>

extern Globals globals;

Monster::Monster(int x, int y, MonsterList * manager) : x(x), y(y), manager(manager) {
}

Monster::~Monster() {
}

MonsterList::MonsterList(MonsterTypeData mtd, Logger * log) : monsterTypeData(mtd) {
  firePattern = parseFirePattern(mtd.firePatternStr);
  if (firePattern.empty()) {
    std::string error_str = "Firepattern for monster \"";
    error_str += monsterTypeData.name;
    error_str += "\" could not be parsed!";
    log->error(error_str.c_str());
  }

  // Convert hitbox description from image file pixel units to screen pixel units
  screenpx_per_imgpx_x = double(monsterTypeData.width) / double(monsterTypeData.monster_img_frame_size.x);
  screenpx_per_imgpx_y = double(monsterTypeData.height) / double(monsterTypeData.monster_img_frame_size.y);
  this->monsterTypeData.hitbox.x *= screenpx_per_imgpx_x;
  this->monsterTypeData.hitbox.w *= screenpx_per_imgpx_x;
  this->monsterTypeData.hitbox.y *= screenpx_per_imgpx_y;
  this->monsterTypeData.hitbox.h *= screenpx_per_imgpx_y;

  this->monsterTypeData.projectile_launch_center.x *= screenpx_per_imgpx_x;
  this->monsterTypeData.projectile_launch_center.y *= screenpx_per_imgpx_y;
}

MonsterList::~MonsterList() {
  for (Monster * monster : monsters) delete monster;
}

int MonsterList::loadMedia(SDL_Renderer * renderer, Logger * log) {
  sprites = loadImage(monsterTypeData.monster_img_file.c_str(),renderer,log);
	if (sprites==nullptr){
		log->error("Error: A monster sprite was not loaded.");
		return -1;
	}

  frameToSpriteRect = new SDL_Rect[monsterTypeData.num_frames];
  for (int i = 0; i < monsterTypeData.num_frames; i++) {
    frameToSpriteRect[i] = {monsterTypeData.monster_img_frame_size.x*i,0,
                            monsterTypeData.monster_img_frame_size.x  , monsterTypeData.monster_img_frame_size.y};
  }
  frameToDeathSpriteRect = new SDL_Rect[monsterTypeData.num_death_frames];
  for (int i = 0; i < monsterTypeData.num_death_frames; i++) {
    frameToDeathSpriteRect[i] = {monsterTypeData.monster_img_frame_size.x*(i+monsterTypeData.num_frames),0,
                            monsterTypeData.monster_img_frame_size.x  , monsterTypeData.monster_img_frame_size.y};
  }

  death_sound_chunk = Mix_LoadWAV(monsterTypeData.death_sound_file.c_str());
  if (death_sound_chunk==nullptr) {
    log->MIX_Error("A monster death sound could not be loaded.");
    return -1;
  }

  return 0;
}

void MonsterList::unloadMedia() {
  Mix_FreeChunk(death_sound_chunk);
  SDL_DestroyTexture(sprites); sprites = nullptr;
  delete [] frameToSpriteRect;
  delete [] frameToDeathSpriteRect;
}

void MonsterList::handleEvent(SDL_Event * event){

}

void MonsterList::update(App * app) {
  for (Monster * monster : monsters) {

    if (!monster->dying && monster->spawnFrames >= NUM_SPAWN_FRAMES) {
      // increment animation frame
      if (app->frame % monsterTypeData.alive_time_per_frame == 0) monster->frame = (monster->frame + 1) % monsterTypeData.num_frames;


      if (monster->firePatternStepCountdown==0) {
        FirePatternStep & fireStep = firePattern[monster->firePatternStepIndex];
        if (fireStep.u>0) fireBullet(monster,DIRECTION_UP,fireStep.u);
        if (fireStep.l>0) fireBullet(monster,DIRECTION_LEFT,fireStep.l);
        if (fireStep.d>0) fireBullet(monster,DIRECTION_DOWN,fireStep.d);
        if (fireStep.r>0) fireBullet(monster,DIRECTION_RIGHT,fireStep.r);

        // increment step index
        ++monster->firePatternStepIndex;
        if (monster->firePatternStepIndex>=firePattern.size()) monster->firePatternStepIndex = 0;

        //reset countdown
        monster->firePatternStepCountdown = fireStep.wait;
      }
      else --monster->firePatternStepCountdown;


      // check if it should start dying
      ContainsBitmask monster_hitbox_contents = app->rectContents(monster->hitbox);
      if ( (monster_hitbox_contents & CONTAINS_DEADLY_EXPLOSION) &&
           !(monster_hitbox_contents & monsterTypeData.protected_by) ) {
        monster->dying = true;
        monster->frame = 0; // we will now start using frame for death animation

        Mix_PlayChannel(-1,death_sound_chunk,0);

        // only award score if player is killer
        if (monster_hitbox_contents & CONTAINS_PLAYER_CAUSED_EXPLOSION) app->addScore(SCORE_FOR_KILLING_MONSTER);
      }
    }
    else if (monster->dying) {
      if (app->frame % monsterTypeData.death_time_per_frame == 0) ++(monster->frame);
      --monster->death_animation_global_frames_remaining;
      if (monster->frame >= monsterTypeData.num_death_frames) monster->erase_this_monster=true;
    }
    else if (monster->spawnFrames < NUM_SPAWN_FRAMES) {
      ++monster->spawnFrames;
      if (app->frame % monsterTypeData.alive_time_per_frame == 0) monster->frame = (monster->frame + 1) % monsterTypeData.num_frames;
    }

  }

  // Erase any dead monsters:
  for (std::list<Monster*>::iterator iter = monsters.begin(); iter != monsters.end(); ++iter)
    if ((*iter)->erase_this_monster) {
      delete (*iter);
      iter = monsters.erase(iter);
    }
}

void MonsterList::render(App * app, SDL_Renderer * renderer) {
  for (Monster * monster : monsters) {
    if (!monster->dying && monster->spawnFrames >= NUM_SPAWN_FRAMES) {
      SDL_RenderCopy(renderer, sprites, &(frameToSpriteRect[monster->frame]), &(monster->rect));
    }
    else if (monster->dying) {
      int alphaMod = std::min((255*monster->death_animation_global_frames_remaining) /
                                  (monsterTypeData.num_fadeout_frames*monsterTypeData.death_time_per_frame),
                              255);
      SDL_SetTextureAlphaMod(sprites,alphaMod);
      SDL_RenderCopy(renderer, sprites, &(frameToDeathSpriteRect[monster->frame]), &(monster->rect));
      SDL_SetTextureAlphaMod(sprites,255);
    }
    else if (monster->spawnFrames < NUM_SPAWN_FRAMES) {
      float spawn_progress = double(monster->spawnFrames)/double(NUM_SPAWN_FRAMES);
      SDL_Rect target_rect(monster->rect);
      target_rect.x += monster->rect.w/2 * (1-spawn_progress);
      target_rect.y += monster->rect.h/2 * (1-spawn_progress);
      target_rect.w *= spawn_progress;
      target_rect.h *= spawn_progress;
      const SDL_Point rotCenter = {int(float(monsterTypeData.projectile_launch_center.x) * spawn_progress),
                                   int(float(monsterTypeData.projectile_launch_center.y) * spawn_progress)};
      SDL_RenderCopyEx(renderer, sprites, &(frameToSpriteRect[monster->frame]), &(target_rect),
                       spawn_progress*360,&rotCenter,SDL_FLIP_NONE);
    }
    // SDL_SetRenderDrawColor(renderer, 255,0,0,255);  //TEST
    // SDL_RenderDrawRect(renderer, &(monster->hitbox)); // TEST
  }
}

void MonsterList::createMonster(int x, int y) {
  Monster * new_monster = new Monster(x,y,this);
  new_monster->rect = {x,y,monsterTypeData.width,monsterTypeData.height};
  new_monster->hitbox = monsterTypeData.hitbox;
  new_monster->hitbox.x += x;
  new_monster->hitbox.y += y;
  new_monster->firePatternStepCountdown = rand() % 150 ; // to avoid overly synced shots between different monsters
  new_monster->firePatternStepIndex = rand() % firePattern.size();
  new_monster->death_animation_global_frames_remaining = monsterTypeData.death_time_per_frame * monsterTypeData.num_death_frames;
  monsters.push_front(new_monster);
}

void MonsterList::fireBullet(Monster * monster, DirectionUDLR dir, int speed) {
  vecI center = vecI(monster->x,monster->y) + monsterTypeData.projectile_launch_center;
  vecI d = globals.directionToUnitVector[dir];
  int projectile_launch_dist_in_screen_units = monsterTypeData.projectile_launch_dist * (dir==DIRECTION_UP||dir==DIRECTION_DOWN
                                                                                         ? screenpx_per_imgpx_y
                                                                                         : screenpx_per_imgpx_x);
  vecI bulletBackCenter = center + projectile_launch_dist_in_screen_units*d;
  vecI bulletCenter = bulletBackCenter + (monsterTypeData.bulletManager->projectileTypeData.height/2)*d;
  vecI bulletTopLeft = bulletCenter - vecI(monsterTypeData.bulletManager->projectileTypeData.width/2,monsterTypeData.bulletManager->projectileTypeData.height/2);
  // that was the top left of the bullet BEFORE it gets rotated by the bullet manager. ugh, this is bad design.
  monsterTypeData.bulletManager->createProjectile(bulletTopLeft.x,bulletTopLeft.y,speed,dir,false,monster);
}
