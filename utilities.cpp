#include "utilities.h"
#include <string>
#include <sstream>


std::vector<std::string> split(const std::string & str, const char delimiter) {

  std::vector<std::string> split_string;

  std::istringstream iss(str);
  std::string token{};
  while(std::getline(iss,token,delimiter))
    split_string.push_back(token);

  return split_string;
}




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



vecI rotatePoint(const vecI & p,const vecI & c,DirectionUDLR dir) {
	switch(dir) {
		case DIRECTION_UP:    return p;
		case DIRECTION_DOWN:  return 2*c-p;
		case DIRECTION_LEFT:  return vecI(p.y-c.y+c.x,c.x+c.y-p.x);
		case DIRECTION_RIGHT: return vecI(c.x+c.y-p.y,p.x-c.x+c.y);
		default:              return p;
	}
}


SDL_Rect rotateRect(const SDL_Rect & r,const vecI & c,DirectionUDLR dir) {
	vecI newTopLeft;
	switch(dir) {
		case DIRECTION_UP:    return r;
		case DIRECTION_DOWN:  newTopLeft = rotatePoint(vecI(r.x+r.w,r.y+r.h),c,dir);
													return {newTopLeft.x,newTopLeft.y,r.w,r.h};
		case DIRECTION_LEFT:  newTopLeft = rotatePoint(vecI(r.x+r.w,r.y),c,dir);
													return {newTopLeft.x,newTopLeft.y,r.h,r.w};
		case DIRECTION_RIGHT: newTopLeft = rotatePoint(vecI(r.x,r.y+r.h),c,dir);
													return {newTopLeft.x,newTopLeft.y,r.h,r.w};
		default:              return r;
	}
}



Globals::Globals() {
	directionToUnitVector[DIRECTION_UP]   =    vecI( 0,-1); // This is like local Y axis
	directionToUnitVector[DIRECTION_DOWN]   =  vecI( 0, 1);
	directionToUnitVector[DIRECTION_LEFT]   =  vecI(-1, 0);
	directionToUnitVector[DIRECTION_RIGHT]   = vecI( 1, 0);
	directionToUnitVector[DIRECTION_NEUTRAL] = vecI( 0, 0);

	directionToLocalX[DIRECTION_UP]   =    vecI( 1, 0);
	directionToLocalX[DIRECTION_DOWN]   =  vecI(-1, 0);
	directionToLocalX[DIRECTION_LEFT]   =  vecI( 0,-1);
	directionToLocalX[DIRECTION_RIGHT]   = vecI( 0, 1);
	directionToLocalX[DIRECTION_NEUTRAL] = vecI( 0, 0);

	directionToRotAngle[DIRECTION_NEUTRAL] = 0;
	directionToRotAngle[DIRECTION_UP] = 0;
	directionToRotAngle[DIRECTION_RIGHT] = 90;
	directionToRotAngle[DIRECTION_DOWN] = 180;
	directionToRotAngle[DIRECTION_LEFT] = 270;

  // Initialize all colors to a noticable ugly purple,
  // so that we remember to manually initialize each palette color below.
  for (int c = 0; c < NUM_PALETTE_COLORS; c++){
    palette[c] = {255, 0, 255, 255} ;
  }
  palette[PALETTE_BLACK] = {0,0,0,255};
  palette[PALETTE_WHITE] = {255,255,255,255};
}
Globals globals;
