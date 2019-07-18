#include "monster.h"
#include "utilities.h"
#include "main.h"

extern Globals globals;

Monster::Monster(int x, int y) : x(x), y(y) {
}

Monster::~Monster() {
}

MonsterList::MonsterList(MonsterTypeData mtd) : monsterTypeData(mtd) {
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

      // TODO let the monster do its thang

      // check if it should start dying
      if (app->rectContents(monster->hitbox) & CONTAINS_DEADLY_EXPLOSION) {
        // TODO update monster's rect to be correct for death
        monster->dying = true;
        monster->frame = 0; // we will now start using frame for death animation
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
  }
}

void MonsterList::createMonster(int x, int y) {
  Monster * new_monster = new Monster(x,y);
  new_monster->rect = {x,y,monsterTypeData.width,monsterTypeData.height};
  new_monster->hitbox = monsterTypeData.hitbox;
  new_monster->hitbox.x += x;
  new_monster->hitbox.y += y;
  monsters.push_front(new_monster);
}
