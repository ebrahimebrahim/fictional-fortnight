#pragma once
#include<string>
#include "textbox.h"
#include "logger.h"

class App; // forward declaration

class HelpScreen {
  public:
    HelpScreen(SDL_Renderer *, TTF_Font *, Logger *);
    ~HelpScreen() {}

    void events(const SDL_Event *);
    void update(App *);
    void render(SDL_Renderer *);

  private:
    SDL_Renderer * renderer = nullptr;
    TTF_Font * font = nullptr;
    Logger * log = nullptr;
    SDL_Color textColor;
    TextBox helpTextBox;

    bool tryExitHelp = false;
};
