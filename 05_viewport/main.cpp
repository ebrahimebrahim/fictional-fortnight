#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include "main.h"



int main( int argc, char* args[] )
{

	App app{};

	if (app.initialize() < 0) {
		app.printError("Error encountered while initializing application. Quitting.", false);
		return -1;
	}

	if (app.loadMedia() < 0) {
		app.printError("Error encountered while loading media. Quitting.", false);
		return -1;
	}

	return app.execute();
}


App::App() {
	for (int state = 0; state < THINGY_NUM_STATES; state++){
		thingyImages[state] = nullptr; // Is there a better way to null-initialize array of pointers?
	}
}


int App::initialize() {
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printError("Error initializing SDL");
		return -1;
	}

	if ( (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::string error_msg = "Error initializing SDL_Image png module; last error message from SDL_Image: ";
		error_msg += IMG_GetError();
		printError(error_msg.c_str(),false);
		return -1;
	}

	window = SDL_CreateWindow( "SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0 );
	if( window == nullptr ) {
		printError("Error creating window");
		return -1;
	}

	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if( renderer == nullptr ) {
		printError("Error creating renderer");
		return -1;
	}


	return 0;
}


App::~App() {
	unloadMedia();
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	IMG_Quit();
	SDL_Quit();
}


int App::execute(){

	while (!quit){
		handleEvents();
		mainLoop();
		render();
		SDL_Delay( 10 );
	}

	return 0;
}


void App::handleEvents(){
	SDL_Event event;

	while(SDL_PollEvent(&event)){

		switch (event.type) {
			case SDL_QUIT:
				quit=true;
				break;
			case SDL_KEYDOWN:
				handleKeypress(&(event.key));
				break;
		}

		if (event.type==SDL_QUIT) {
			quit=true;
		}
	}
}


void App::mainLoop(){

	if (tryMoveState!=TRY_MOVE_STATE_NOT_TRYING){
		switch (tryMoveState) {
			case TRY_MOVE_STATE_UP:
				if (pos.y > 0) pos.y--;
				break;
			case TRY_MOVE_STATE_DOWN:
				if (pos.y < NUM_TILES_Y-1) pos.y++;
				break;
			case TRY_MOVE_STATE_LEFT:
				if (pos.x > 0) pos.x--;
				break;
			case TRY_MOVE_STATE_RIGHT:
				if (pos.x < NUM_TILES_X-1) pos.x++;
				break;
			default: break;
		}
		tryMoveState=TRY_MOVE_STATE_NOT_TRYING;
	}

	SDL_Rect right_menu_rect = {SCREEN_WIDTH,0,RIGHT_MENU_WIDTH,WINDOW_HEIGHT};
	SDL_RenderSetViewport(renderer,&right_menu_rect);
	SDL_SetRenderDrawColor(renderer,100,100,50,255);
	SDL_RenderFillRect(renderer,nullptr);

	SDL_Rect screen_rect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
  SDL_RenderSetViewport(renderer,&screen_rect);
	SDL_SetRenderDrawColor(renderer,50,50,50,255);
	SDL_RenderFillRect(renderer,nullptr);

	SDL_Rect r = {pos.x*TILE_WIDTH,pos.y*TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT};
	SDL_RenderCopy(renderer, thingyImages[thingyState], nullptr, &r);
}


void App::render(){
	SDL_RenderPresent( renderer );
}


void App::printError(const char * msg, bool include_sdl_error) {
	if (include_sdl_error)
		printf( "%s ; here's the latest SDL error: %s\n",msg, SDL_GetError() );
	else
		printf("%s\n",msg);
}


int App::loadMedia(){

	const char * thingyStateToFilename [THINGY_NUM_STATES];
	thingyStateToFilename[THINGY_UP]="up.png";
	thingyStateToFilename[THINGY_DOWN]="down.png";
	thingyStateToFilename[THINGY_LEFT]="left.png";
	thingyStateToFilename[THINGY_RIGHT]="right.png";
	thingyStateToFilename[THINGY_NEUTRAL]="neutral.png";

	for (int state = 0; state < THINGY_NUM_STATES; state++){

		thingyImages[state] = loadImage(thingyStateToFilename[state]);
		if (thingyImages[state] == nullptr){
			printError("Error: Some media was not loaded.",0);
			return -1;
		}

	}

	return 0;
}


SDL_Texture	 * App::loadImage(const char * filename){
	SDL_Surface * img = IMG_Load(filename);
	if (img==nullptr){
		std::string error_msg = "Error while loading ";
		error_msg += filename;
		printError(error_msg.c_str());
		return nullptr;
	}
	SDL_Texture * img_texture = SDL_CreateTextureFromSurface(renderer,img);
	SDL_FreeSurface(img);
	if (img_texture == nullptr){
		std::string error_msg = "Error while creating texture from ";
		error_msg += filename;
		printError(error_msg.c_str());
		return nullptr;
	}
	return img_texture;
}


void App::unloadMedia(){
	for (int state = 0; state < THINGY_NUM_STATES; state++){
		SDL_DestroyTexture(thingyImages[state]);
		thingyImages[state] = nullptr;
	}
}

void App::handleKeypress(SDL_KeyboardEvent * key){
	switch (key->keysym.scancode) {
		case SDL_SCANCODE_LEFT:
			thingyState = THINGY_LEFT;
			tryMoveState = TRY_MOVE_STATE_LEFT;
			break;
		case SDL_SCANCODE_RIGHT:
			thingyState = THINGY_RIGHT;
			tryMoveState = TRY_MOVE_STATE_RIGHT;
			break;
		case SDL_SCANCODE_UP:
			thingyState = THINGY_UP;
			tryMoveState = TRY_MOVE_STATE_UP;
			break;
		case SDL_SCANCODE_DOWN:
			thingyState = THINGY_DOWN;
			tryMoveState = TRY_MOVE_STATE_DOWN;
			break;
		case SDL_SCANCODE_Q:
			quit=true;
			break;
		default:
			break;
	}
}
