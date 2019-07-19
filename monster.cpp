#include "monster.h"
#include "utilities.h"
#include "main.h"
#include <cstdlib>

extern Globals globals;

Monster::Monster(int x, int y) : x(x), y(y) {
}

Monster::~Monster() {
}

MonsterList::MonsterList(MonsterTypeData mtd) : monsterTypeData(mtd) {
  firePattern = parseFirePattern(mtd.firePatternStr);

  // Convert hitbox description from image file pixel units to screen pixel units
  double screenpx_per_imgpx_x = double(monsterTypeData.width) / double(monsterTypeData.monster_img_frame_size.x);
  double screenpx_per_imgpx_y = double(monsterTypeData.height) / double(monsterTypeData.monster_img_frame_size.y);
  this->monsterTypeData.hitbox.x *= screenpx_per_imgpx_x;
  this->monsterTypeData.hitbox.w *= screenpx_per_imgpx_x;
  this->monsterTypeData.hitbox.y *= screenpx_per_imgpx_y;
  this->monsterTypeData.hitbox.h *= screenpx_per_imgpx_y;
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


  return 0;
}

void MonsterList::unloadMedia() {
  SDL_DestroyTexture(sprites); sprites = nullptr;
  delete frameToSpriteRect;
  delete frameToDeathSpriteRect;
}

void MonsterList::handleEvent(SDL_Event * event){

}

void MonsterList::update(App * app) {
  for (Monster * monster : monsters) {

    if (!monster->dying) {
      // increment animation frame
      if (app->frame % 10 == 0) monster->frame = (monster->frame + 1) % monsterTypeData.num_frames;


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
      if (app->rectContents(monster->hitbox) & CONTAINS_DEADLY_EXPLOSION) {
        monster->dying = true;
        monster->frame = 0; // we will now start using frame for death animation
        app->addScore(5);
      }
    }
    else {
      if (app->frame % 10 == 0) ++(monster->frame);
      if (monster->frame >= monsterTypeData.num_death_frames) monster->erase_this_monster=true;
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
    if (!monster->dying) {
      SDL_RenderCopy(renderer, sprites, &(frameToSpriteRect[monster->frame]), &(monster->rect));
    }
    else {
      SDL_RenderCopy(renderer, sprites, &(frameToDeathSpriteRect[monster->frame]), &(monster->rect));
    }
    // SDL_SetRenderDrawColor(renderer, 255,0,0,255);  //TEST
    // SDL_RenderDrawRect(renderer, &(monster->hitbox)); // TEST
  }
}

void MonsterList::createMonster(int x, int y) {
  Monster * new_monster = new Monster(x,y);
  new_monster->rect = {x,y,monsterTypeData.width,monsterTypeData.height};
  new_monster->hitbox = monsterTypeData.hitbox;
  new_monster->hitbox.x += x;
  new_monster->hitbox.y += y;
  new_monster->firePatternStepCountdown = rand() % 100 ; // to avoid overly synced shots between different monsters
  monsters.push_front(new_monster);
}

void MonsterList::fireBullet(Monster * monster, DirectionUDLR dir, int speed) {
  vecI center = vecI(monster->x,monster->y) + vecI(monsterTypeData.width/2,monsterTypeData.height/2);
  vecI d = globals.directionToUnitVector[dir];
  vecI bulletBackCenter = center + (abs(dot(d,vecI(monsterTypeData.width/2,monsterTypeData.height/2)))+5)*d;
  // (there 5 is the size of the gap between monster and spawned bullet)
  vecI bulletCenter = bulletBackCenter + (monsterTypeData.bulletManager->projectileTypeData.height/2)*d;
  vecI bulletTopLeft = bulletCenter - vecI(monsterTypeData.bulletManager->projectileTypeData.width/2,monsterTypeData.bulletManager->projectileTypeData.height/2);
  // that was the top left of the bullet BEFORE it gets rotated by the bullet manager. ugh, this is bad design.
  monsterTypeData.bulletManager->createProjectile(bulletTopLeft.x,bulletTopLeft.y,speed,dir);
}
