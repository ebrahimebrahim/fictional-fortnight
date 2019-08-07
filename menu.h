#pragma once
#include<string>
#include<vector>
#include<functional>
#include "logger.h"

class App; // forward declaration

class Menu {
  public:
    Menu(const char * title, SDL_Renderer *, TTF_Font *, Logger * log);
    virtual ~Menu();
    virtual void setFont(TTF_Font * font) {this->font=font;}
    virtual void nextOption();
    virtual void prevOption();


    virtual void addItem(const char *, std::function<void ()>);

    virtual void events(const SDL_Event *);
    virtual void update(App *);
    virtual void render(SDL_Renderer *);

    std::string title;

  protected:
    std::vector<std::string> items;
    std::vector<std::function<void ()> > actions;
    std::vector<SDL_Texture *> itemTextTextures;
    SDL_Texture * titleTextTexture;
    int selected = -1;
    SDL_Renderer * renderer = nullptr;
    TTF_Font * font = nullptr;
    Logger * log;
    SDL_Color textColor;

    const int topleft_x = 150;
    const int topleft_y = 150;
};
