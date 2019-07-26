#pragma once
#include<string>
#include<vector>
#include<functional>
#include "logger.h"

class App; // forward declaration

class Menu {
  public:
    Menu(const char * title, SDL_Renderer *, TTF_Font *, Logger * log);
    ~Menu();
    void setFont(TTF_Font * font) {this->font=font;}
    void nextOption();
    void prevOption();


    void addItem(const char *, std::function<void ()>);

    void events(const SDL_Event *);
    void update(App *);
    void render(SDL_Renderer *);

    std::string title;

  private:
    std::vector<std::string> items;
    std::vector<std::function<void ()> > actions;
    std::vector<SDL_Texture *> itemTextTextures;
    SDL_Texture * titleTextTexture;
    int selected = -1;
    SDL_Renderer * renderer = nullptr;
    TTF_Font * font = nullptr;
    Logger * log;
    SDL_Color textColor;
};
