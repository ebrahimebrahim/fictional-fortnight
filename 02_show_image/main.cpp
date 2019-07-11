#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
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

	SDL_FillRect( screenSurface, nullptr, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
	SDL_BlitScaled(peupImage,nullptr,screenSurface,nullptr);

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
		if (event.type==SDL_QUIT) {
			quit=true;
		}
	}
}


void App::mainLoop(){

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
	peupImage = IMG_Load("peup.png");
	if (peupImage==nullptr) {
		printError("Error while loading peup.png");
		return -1;
	}
	return 0;
}

void App::unloadMedia(){
	SDL_FreeSurface(peupImage);
	peupImage = nullptr;
}
