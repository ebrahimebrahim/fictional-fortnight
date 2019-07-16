#pragma once
#include <SDL2/SDL.h>
#include "entity.h"
#include "utilities.h"






class PlayerEntity :  public Entity {
  public:
    PlayerEntity();
    ~PlayerEntity();
    virtual int loadMedia(SDL_Renderer *,Logger *);
    virtual void unloadMedia();
    virtual void handleEvent(SDL_Event *);
    virtual void update(App *); // Update step to go into main game loop
    virtual void render(App *, SDL_Renderer *);

    // Assets
    SDL_Texture * sprites = nullptr;

    // Useful tables
    SDL_Rect orientationToSpriteRect [DIRECTION_NUM_STATES];
    DirectionUDLR directionalKeyToPlayerDirection(SDL_Scancode);

    // State
    DirectionUDLR orientation = DIRECTION_NEUTRAL;
    DirectionUDLR tryMove = DIRECTION_NEUTRAL;
    CircleBuffer<SDL_Scancode> lastDirectionalKeys;
    bool tryShoot = false;
    int x = 0; // x,y of top left corner of the player entity
    int y = 0;

    // Constants (for now)
    int width = 20;
    int height = 20;
    int v = 6;


};
