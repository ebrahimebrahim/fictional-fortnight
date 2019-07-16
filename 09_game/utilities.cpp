#include "utilities.h"



SDL_Texture	 *  loadImage(const char * filename, SDL_Renderer * renderer, Logger * log){
	SDL_Surface * img = IMG_Load(filename);
	if (img==nullptr){
		std::string error_msg = "Error while loading ";
		error_msg += filename;
		if (log) log->SDL_Error(error_msg.c_str());
		return nullptr;
	}

  // Make pure white be transparent.
	SDL_SetColorKey(img,SDL_TRUE,SDL_MapRGB(img->format,255,255,255));

	SDL_Texture * img_texture = SDL_CreateTextureFromSurface(renderer,img);
	SDL_FreeSurface(img);
	if (img_texture == nullptr){
		std::string error_msg = "Error while creating texture from ";
		error_msg += filename;
		if (log) log->SDL_Error(error_msg.c_str());
		return nullptr;
	}
	return img_texture;
}


bool isArrowKey(SDL_Scancode scancode) {
return ( scancode == SDL_SCANCODE_LEFT  ||
				 scancode == SDL_SCANCODE_RIGHT ||
			   scancode == SDL_SCANCODE_UP    ||
				 scancode == SDL_SCANCODE_DOWN      );
}



Tables::Tables() {
	directionToUnitVector[DIRECTION_UP]   =    vecI( 0,-1);
	directionToUnitVector[DIRECTION_DOWN]   =  vecI( 0, 1);
	directionToUnitVector[DIRECTION_LEFT]   =  vecI(-1, 0);
	directionToUnitVector[DIRECTION_RIGHT]   = vecI( 1, 0);
	directionToUnitVector[DIRECTION_NEUTRAL] = vecI( 0, 0);
}
Tables gTables;
