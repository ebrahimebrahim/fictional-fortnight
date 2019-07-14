#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include "main.h"



int main( int argc, char* args[] )
{

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


App::App() : log{} {

}


int App::initialize() {
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
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


	if ( SDL_NumJoysticks() > 0 ) {
		joystick = SDL_JoystickOpen(0); // If this fails then joystick remains as nullptr
		// we are okay with this failure case because it would be as though SDL_NumJoysticks() were 0
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
	SDL_JoystickClose(joystick);
	SDL_Quit();
}


int App::execute(){

	while (!quit){
		handleEvents();
		mainLoop();
		render();
		SDL_Delay( 20 );
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
			case SDL_JOYHATMOTION:
				handleJoyhat(&(event.jhat));
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

	// Set the renderer's viewport to the left hand screen
  SDL_RenderSetViewport(renderer,&screen_rect);

	SDL_SetRenderDrawColor(renderer,80,80,80,255);
	SDL_RenderFillRect(renderer,nullptr);

	SDL_Rect thingy_rect = {pos.x*TILE_WIDTH,pos.y*TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT};
	SDL_RenderCopy(renderer, thingySprites, &(thingyStateToSpriteRect[thingyState]), &thingy_rect);

	SDL_RenderPresent( renderer );
}


int App::loadMedia(){

	// Load textures
	thingySprites = loadImage("thingy.png");
	if (thingySprites==nullptr){
		log.error("Error: Some media was not loaded.");
		return -1;
	}
	thingyStateToSpriteRect[THINGY_NEUTRAL]={0,0,5,5};
	thingyStateToSpriteRect[THINGY_UP]={5,0,5,5};
	thingyStateToSpriteRect[THINGY_DOWN]={10,0,5,5};
	thingyStateToSpriteRect[THINGY_LEFT]={15,0,5,5};
	thingyStateToSpriteRect[THINGY_RIGHT]={20,0,5,5};


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
	timerTextBox->updateText("TIMER");


	return 0;
}


SDL_Texture	 * App::loadImage(const char * filename){
	SDL_Surface * img = IMG_Load(filename);
	if (img==nullptr){
		std::string error_msg = "Error while loading ";
		error_msg += filename;
		log.SDL_Error(error_msg.c_str());
		return nullptr;
	}

	SDL_SetColorKey(img,SDL_TRUE,SDL_MapRGB(img->format,255,255,255));

	SDL_Texture * img_texture = SDL_CreateTextureFromSurface(renderer,img);
	SDL_FreeSurface(img);
	if (img_texture == nullptr){
		std::string error_msg = "Error while creating texture from ";
		error_msg += filename;
		log.SDL_Error(error_msg.c_str());
		return nullptr;
	}
	return img_texture;
}


void App::unloadMedia(){

	SDL_DestroyTexture(thingySprites);
	thingySprites = nullptr;

	delete peupTextBox;

	TTF_CloseFont(font);
	font = nullptr;

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

void App::handleJoyhat(SDL_JoyHatEvent * jhat){
	SDL_KeyboardEvent fake_keyboard_event; // This is a terrible hack but I'm just testing joystick stuff lol
	switch (jhat->value) {
		case SDL_HAT_UP:
			fake_keyboard_event.keysym.scancode = SDL_SCANCODE_UP;
			break;
		case SDL_HAT_DOWN:
			fake_keyboard_event.keysym.scancode = SDL_SCANCODE_DOWN;
			break;
		case SDL_HAT_LEFT:
			fake_keyboard_event.keysym.scancode = SDL_SCANCODE_LEFT;
			break;
		case SDL_HAT_RIGHT:
			fake_keyboard_event.keysym.scancode = SDL_SCANCODE_RIGHT;
			break;
		default: return;
	}
	handleKeypress(&fake_keyboard_event);
}
