#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include "main.h"



int main( int argc, char* args[] ) {

	App app{};

	if (app.initialize() < 0) {
		app.log.error("Error encountered while initializing application. Quitting.");
		return -1;
	}

	if (app.loadMedia() < 0) {
		app.log.error("Error encountered while loading media. Quitting.");
		return -1;
	}

	return app.execute();
}


App::App() : log{}, playerEntity{} {
}


int App::initialize() {
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		log.SDL_Error("Error initializing SDL");
		return -1;
	}

	if ( (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		log.IMG_Error("Error initializing SDL_Image png module");
		return -1;
	}

	if (TTF_Init()==-1) {
		log.TTF_Error("Error initializing SDL_ttf");
		return -1;
	}


	window = SDL_CreateWindow( "SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0 );
	if( window == nullptr ) {
		log.SDL_Error("Error creating window");
		return -1;
	}

	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if( renderer == nullptr ) {
		log.SDL_Error("Error creating renderer");
		return -1;
	}



	// Initialize all colors to a noticable ugly purple,
	// so that we remember to manually initialize each palette color below.
	for (int c = 0; c < NUM_PALETTE_COLORS; c++){
		palette[c] = {255, 0, 255, 255} ;
	}
	palette[PALETTE_BLACK] = {0,0,0,255};
	palette[PALETTE_WHITE] = {255,255,255,255};



	return 0;
}


App::~App() {
	unloadMedia();
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}


int App::execute(){

	while (!quit){
		Uint32 t0 = SDL_GetTicks();
		handleEvents();
		mainLoop();
		render();
		Uint32 dt = SDL_GetTicks()-t0;
		SDL_Delay( dt<20 ? 20-dt : 0 );
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

		playerEntity.handleEvent(&event);
		projectileList.handleEvent(&event);

	}
}


void App::mainLoop(){

	playerEntity.update(this);
	projectileList.update(this);

}


void App::render(){

	// Set the renderer's viewport to the right hand menu
	SDL_RenderSetViewport(renderer,&right_menu_rect);

	SDL_SetRenderDrawColor(renderer,100,100,50,255);
	SDL_RenderFillRect(renderer,nullptr);
	peupTextBox->renderCopy(10,10);
	std::string timeString = std::to_string((SDL_GetTicks() - timerStart)/100);
	timerTextBox->updateText(timeString.c_str());
	timerTextBox->renderCopy(10,50);
	std::string fpsString = "FPS: ";
	fpsString += std::to_string(fps);
	if (frame % 10 ==0 ) fpsTextBox->updateText(fpsString.c_str());
	fpsTextBox->renderCopy(10,90);

	// Set the renderer's viewport to the left hand screen
  SDL_RenderSetViewport(renderer,&screen_rect);

	SDL_SetRenderDrawColor(renderer,80,80,80,255);
	SDL_RenderFillRect(renderer,nullptr);

	playerEntity.render(this,renderer);
	projectileList.render(this,renderer);


	SDL_RenderPresent( renderer );
	int newFrameTime = SDL_GetTicks();
	fps = 1000./float(newFrameTime - lastFrameTime);
	lastFrameTime = newFrameTime;
	frame++;
}


int App::loadMedia(){

	// Load whatever is needed by entities
	if (playerEntity.loadMedia(renderer, &log)<0){
		log.error("Error: Some media was not loaded.");
		return -1;
	}
	if (projectileList.loadMedia(renderer, &log)<0){
		log.error("Error: Some media was not loaded.");
		return -1;
	}

	//Load fonts
	font  = TTF_OpenFont("Roboto-Regular.ttf",14);
	if (font==nullptr){
		log.TTF_Error("Error loading font");
		return -1;
	}

	//Initialize text Textboxes
	peupTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	peupTextBox->updateText("peup.");
	timerTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	timerTextBox->updateText("[TIMER]");
	fpsTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	fpsTextBox->updateText("[FPS]");


	return 0;
}




void App::unloadMedia(){

	playerEntity.unloadMedia();
	projectileList.unloadMedia();

	delete peupTextBox;
	delete timerTextBox;
	delete fpsTextBox;

	TTF_CloseFont(font);
	font = nullptr;

}

void App::handleKeypress(SDL_KeyboardEvent * key){

	switch (key->keysym.scancode) {
		case SDL_SCANCODE_SPACE:
			timerStart = SDL_GetTicks();
			break;
		case SDL_SCANCODE_Q:
			quit=true;
			break;
		default:
			break;
	}

}
