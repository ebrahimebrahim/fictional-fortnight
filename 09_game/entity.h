#pragma once
#include <SDL2/SDL.h>
#include "logger.h"


class App; // Forward declaration


class Entity {
  public:
    Entity() {}
    ~Entity() {}
    virtual int loadMedia(SDL_Renderer *,Logger *) {return 0;}
    virtual void unloadMedia() {}
    virtual void handleEvent(SDL_Event *) {}
    virtual void update(App *) {} // Update step to go into main game loop
    virtual void render(App *, SDL_Renderer *) {}
};
