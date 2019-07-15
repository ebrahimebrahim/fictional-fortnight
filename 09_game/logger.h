#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>

class Logger{
  public:
    Logger();
    ~Logger();
    void error(const char *);
    void SDL_Error(const char *);
    void IMG_Error(const char *);
    void TTF_Error(const char *);
};
