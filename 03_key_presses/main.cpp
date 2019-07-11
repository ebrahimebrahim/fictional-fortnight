#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include "main.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
	window = SDL_CreateWindow( "SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
	if( window == nullptr ) {
		printError("Error creating window");
		return -1;
	}
	screenSurface = SDL_GetWindowSurface( window );
	return 0;
}


App::~App() {
	unloadMedia();
	SDL_DestroyWindow( window );
	SDL_Quit();
}


int App::execute(){

	while (!quit){
		handleEvents();
		mainLoop();
		render();
		SDL_Delay( 100 );
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
	SDL_BlitScaled(thingyImages[thingyState],nullptr,screenSurface,nullptr);
}


void App::render(){
	SDL_UpdateWindowSurface( window );
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
		thingyImages[state] = IMG_Load(thingyStateToFilename[state]);
		if (thingyImages[state] == nullptr){
			std::string error_msg = "Error while loading ";
			error_msg += thingyStateToFilename[state];
			printError(error_msg.c_str());
			return -1;
		}
	}

	return 0;
}

void App::unloadMedia(){
	for (int state = 0; state < THINGY_NUM_STATES; state++){
		SDL_FreeSurface(thingyImages[state]);
		thingyImages[state] = nullptr;
	}
}

void App::handleKeypress(SDL_KeyboardEvent * key){
	SDL_Scancode & scancode = key->keysym.scancode;
	printf("Key pressed: %s\n",(scancode==SDL_SCANCODE_LEFT)?"is left":"isn't left");
}
