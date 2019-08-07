#include "menu.h"
#include "main.h"
#include "utilities.h"
#include <string>

extern Globals globals;

LoseMenu::LoseMenu(const char * title,
           SDL_Renderer * renderer,
           TTF_Font * font,
           Logger * log) :
             Menu(title, renderer, font, log),
             killerTextBox(font,&(globals.palette[PALETTE_BLACK]),renderer,log)
{

}

void LoseMenu::render(SDL_Renderer * renderer) {
  Menu::render(renderer);
  killerTextBox.renderCopy(topleft_x,topleft_y-2*killerTextBox.getTextHeight());
}

void LoseMenu::setKiller(const char * killerName) {
  std::string s = "Killed by: ";
  s+=killerName;
  killerTextBox.updateText(s.c_str());
}
