#pragma once
#include "menu.h"
#include "textbox.h"


class LoseMenu : public Menu {
  public:
    LoseMenu(const char * title, SDL_Renderer *, TTF_Font *, Logger * log);
    void render(SDL_Renderer *);

    void setKiller(const char * killer);

  private:
    TextBox killerTextBox;
};
