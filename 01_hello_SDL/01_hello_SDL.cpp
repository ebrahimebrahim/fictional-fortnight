#include <SDL2/SDL.h>
#include <stdio.h>
#include "01_hello_SDL.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main( int argc, char* args[] )
{

	App app{};

	if (app.initialize() < 0) {
		app.printError("Error encountered while initializing application. Quitting.", false);
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
	if( window == NULL ) {
		printError("Error creating window");
		return -1;
	}
	screenSurface = SDL_GetWindowSurface( window );
	return 0;
}


App::~App() {
	SDL_DestroyWindow( window );
	SDL_Quit();
}


int App::execute(){
	SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
	SDL_UpdateWindowSurface( window );
	SDL_Delay( 500 );
	return 0;
}


void App::printError(const char * msg, bool include_sdl_error) {
	if (include_sdl_error)
		printf( "%s ; here's the latest SDL error: %s\n",msg, SDL_GetError() );
	else
		printf("%s\n",msg);
}
