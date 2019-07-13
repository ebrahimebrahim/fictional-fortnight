#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
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

}


int App::initialize() {
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO ) < 0 ) {
		printError("Error initializing SDL");
		return -1;
	}

	if ( (IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::string error_msg = "Error initializing SDL_Image png module; last error message from SDL_Image: ";
		error_msg += IMG_GetError();
		printError(error_msg.c_str(),false);
		return -1;
	}

	if (TTF_Init()==-1) {
		std::string error_msg = "Error initializing SDL_ttf; last error message from SDL_ttf: ";
		error_msg += TTF_GetError();
		printError(error_msg.c_str(),false);
		return -1;
	}


	if ( (Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3) != MIX_INIT_MP3 ) {
		std::string error_msg = "Error initializing SDL_Mixer mp3 module; last error message from SDL_Mixer: ";
		error_msg += Mix_GetError();
		printError(error_msg.c_str(),false);
		return -1;
	}
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		std::string error_msg = "Error initializing SDL_Mixer; last error message from SDL_Mixer: ";
		error_msg += Mix_GetError();
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


	if ( SDL_NumJoysticks() > 0 ) {
		joystick = SDL_JoystickOpen(0); // If this fails then joystick remains as nullptr
		// we are okay with this failure case because it would be as though SDL_NumJoysticks() were 0
	}


	return 0;
}


App::~App() {
	unloadMedia();
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	IMG_Quit();
	TTF_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_JoystickClose(joystick);
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

	// Render text.
	// This should all be its own class
	// The class would initialize the TTF_Font and destroy it at the end,
	// and it would have a method that can take text and and a renderer and some options and do the rendering
	// fancy feature: allow multiple lines and do wrapping for long lines
	// (The class could be called TextBox or MsgBox or something and it would be set to render to a specific place maybe)
	// fancy feature: cache the texture (whcih it also destroys later) so that if it's called with the same msg it can reuse it.
	// essential feature: properly handle error cases like when nullptr is returned by the various functions there
	SDL_Color text_color = {0,0,0,255};
	SDL_Surface * text_surface = TTF_RenderText_Blended(font, "peup",text_color);
	SDL_Texture * text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
	SDL_FreeSurface(text_surface);
	int w{}, h{};
	TTF_SizeText(font,"peup",&w,&h); // You should also be able to get text size as an attribute of text_surface, which might be better
	SDL_Rect text_rect = {10,10,w,h};
	SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);
	SDL_DestroyTexture(text_texture);

	// Set the renderer's viewport to the left hand screen
  SDL_RenderSetViewport(renderer,&screen_rect);
	SDL_SetRenderDrawColor(renderer,80,80,80,255);
	SDL_RenderFillRect(renderer,nullptr);

	SDL_Rect thingy_rect = {pos.x*TILE_WIDTH,pos.y*TILE_HEIGHT,TILE_WIDTH,TILE_HEIGHT};
	SDL_RenderCopy(renderer, thingySprites, &(thingyStateToSpriteRect[thingyState]), &thingy_rect);

	SDL_RenderPresent( renderer );
}


void App::printError(const char * msg, bool include_sdl_error) {
	if (include_sdl_error)
		printf( "%s ; here's the latest SDL error: %s\n",msg, SDL_GetError() );
	else
		printf("%s\n",msg);
}


int App::loadMedia(){

	// Load textures
	thingySprites = loadImage("thingy.png");
	if (thingySprites==nullptr){
		printError("Error: Some media was not loaded.",false);
		return -1;
	}
	thingyStateToSpriteRect[THINGY_NEUTRAL]={0,0,5,5};
	thingyStateToSpriteRect[THINGY_UP]={5,0,5,5};
	thingyStateToSpriteRect[THINGY_DOWN]={10,0,5,5};
	thingyStateToSpriteRect[THINGY_LEFT]={15,0,5,5};
	thingyStateToSpriteRect[THINGY_RIGHT]={20,0,5,5};


	//Load fonts
	font  = TTF_OpenFont("kano.otf",12);
	if (font==nullptr){
		std::string error_msg = "Error loading font; last error message from SDL_ttf: ";
		error_msg += TTF_GetError();
		printError(error_msg.c_str(),false);
		return -1;
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

	SDL_SetColorKey(img,SDL_TRUE,SDL_MapRGB(img->format,255,255,255));

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

	SDL_DestroyTexture(thingySprites);
	thingySprites = nullptr;

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
