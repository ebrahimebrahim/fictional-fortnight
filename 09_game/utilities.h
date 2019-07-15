#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "logger.h"



enum PaletteColor {
  PALETTE_BLACK,
  PALETTE_WHITE,
  NUM_PALETTE_COLORS // Count-- must be last
};


SDL_Texture	 *  loadImage(const char * filename, SDL_Renderer * renderer, Logger * log = nullptr);
 // load a single image, returning nullptr on fail
 // if logger is passed (optional), then more specific SDL_Image errors will be logged.
