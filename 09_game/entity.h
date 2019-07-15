#pragma once
#include <SDL2/SDL.h>
#include "logger.h"


class App; // Forward declaration


//TODO: Come up with a better name than Entity. It more represents a type of entity rather than one entity.
// e.g. an instance of this class could be many actual "entities"
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
