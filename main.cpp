#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
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


App::App() : log{}
{
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


	// Initialize random number generator
	srand(time(nullptr));



	// --- Create EntityManager objects ---
	playerEntity = new PlayerEntity(); // will be freed when all of entityManagers items are freed in ~App()
	entityManagers_nonprojectile.push_back(playerEntity);

	ProjectileTypeData missile;
	missile.num_frames = 2;
	missile.num_explosion_frames = 12;
	missile.num_deadly_explosion_frames = 4;
	missile.num_fadeout_frames = 8;
	missile.projectile_img_file = "missile.png";
	missile.explosion_img_file = "missile_explode.png";
	missile.projectile_img_frame_size = {16,49};
	missile.explosion_img_frame_size = {80,80};
	missile.width = 16;
	missile.height = 49;
	missile.explosion_width = 100;
	missile.explosion_height = 100;
	missile.projectile_hitbox = {0,0,16,30};
	missile.explosion_hitbox  = {26,25,33,33};
	missile.projectile_detonation_point = {8,5};
	missile.explosion_detonation_point = {38,39};
	missile.explosion_time_per_frame = 5;
	projectileList = new ProjectileList(missile);
	entityManagers_projectile.push_back(projectileList);

	ProjectileTypeData monster1bullets;
	monster1bullets.num_frames = 2;
	monster1bullets.num_explosion_frames = 5;
	monster1bullets.num_deadly_explosion_frames = 1;
	monster1bullets.num_fadeout_frames = 4;
	monster1bullets.projectile_img_file = "monster1bullet.png";
	monster1bullets.explosion_img_file = "monster1bullet_explode.png";
	monster1bullets.projectile_img_frame_size = {7,28};
	monster1bullets.explosion_img_frame_size = {47,51};
	monster1bullets.width = 7;
	monster1bullets.height = 28;
	monster1bullets.explosion_width = 47;
	monster1bullets.explosion_height = 51;
	monster1bullets.projectile_hitbox = {0,0,7,28};
	monster1bullets.explosion_hitbox  = {19,10,7,26};
	monster1bullets.projectile_detonation_point = {0,0};
	monster1bullets.explosion_detonation_point = {19,10};
	monster1bullets.explosion_time_per_frame = 5;
	monster1bulletList = new ProjectileList(monster1bullets);
	entityManagers_projectile.push_back(monster1bulletList);

	MonsterTypeData monster1;
	monster1.name = "Gem";
	monster1.monster_img_file = "monster1.png";
	monster1.num_frames = 5;
	monster1.alive_time_per_frame = 10;
	monster1.num_death_frames = 8;
	monster1.num_fadeout_frames = 4;
	monster1.death_time_per_frame = 10;
	monster1.monster_img_frame_size = vecI(5,5);
	monster1.width  = 20;
	monster1.height = 20;
	monster1.hitbox = {1,1,3,3};
	monster1.bulletManager = monster1bulletList;
	monster1.firePatternStr = "U:10;20 L:10;20 D:10;20 R:10;20";
	monster1List = new MonsterList(monster1);
	entityManagers_nonprojectile.push_back(monster1List);
	entityManagers_monster.push_back(monster1List);



	// ---


	num_levels = entityManagers_monster.size();




	return 0;
}


App::~App() {
	unloadMedia();
	for (EntityManager * entityManager : entityManagers_projectile)
		delete entityManager;
	for (EntityManager * entityManager : entityManagers_nonprojectile)
		delete entityManager;
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

		for (EntityManager * entityManager : entityManagers_projectile)
			entityManager->handleEvent(&event);
		for (EntityManager * entityManager : entityManagers_nonprojectile)
			entityManager->handleEvent(&event);

	}
}


void App::mainLoop(){

	// Let all entity managers do their updates
	for (EntityManager * entityManager : entityManagers_projectile)
		entityManager->update(this);
	for (EntityManager * entityManager : entityManagers_nonprojectile)
		entityManager->update(this);

	// Maybe spawn a monster
	if (rand() % 200 == 0) spawnMonster();

	// Check if level increases or win state changes
	if (score >= level * SCORE_PER_LEVEL_ADVANCE) ++level;
	if (level > num_levels) won=true;
	if (score < SCORE_TO_LOSE) lost=true;



}


void App::render(){

	// Set the renderer's viewport to the right hand menu
	SDL_RenderSetViewport(renderer,&info_area_rect);

	SDL_SetRenderDrawColor(renderer,100,100,50,255);
	SDL_RenderFillRect(renderer,nullptr);
	scoreTextBox->renderCopy(10,10);
	std::string fpsString = "FPS: ";
	fpsString += std::to_string(fps);
	if (frame % 10 ==0 ) fpsTextBox->updateText(fpsString.c_str());
	fpsTextBox->renderCopy(90,10);

	// Set the renderer's viewport to the left hand screen
  SDL_RenderSetViewport(renderer,&screen_rect);

	SDL_SetRenderDrawColor(renderer,80,80,80,255);
	SDL_RenderFillRect(renderer,nullptr);



	for (EntityManager * entityManager : entityManagers_nonprojectile) {
		entityManager->render(this,renderer);
	}
	for (EntityManager * entityManager : entityManagers_projectile) {
		entityManager->render(this,renderer);
	}



	SDL_RenderPresent( renderer );
	int newFrameTime = SDL_GetTicks();
	fps = 1000./float(newFrameTime - lastFrameTime);
	lastFrameTime = newFrameTime;
	frame++;
}


int App::loadMedia(){

	// Load whatever is needed by entities
	for (EntityManager * entityManager : entityManagers_projectile) {
		if (entityManager->loadMedia(renderer, &log)<0){
			log.error("Error: Some media was not loaded.");
			return -1;
		}
	}
	for (EntityManager * entityManager : entityManagers_nonprojectile) {
		if (entityManager->loadMedia(renderer, &log)<0){
			log.error("Error: Some media was not loaded.");
			return -1;
		}
	}

	//Load fonts
	font  = TTF_OpenFont("Roboto-Regular.ttf",14);
	if (font==nullptr){
		log.TTF_Error("Error loading font");
		return -1;
	}

	//Initialize text Textboxes
	scoreTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	updateScoreTextBox();
	fpsTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	fpsTextBox->updateText("[FPS]");


	return 0;
}




void App::unloadMedia(){

	for (EntityManager * entityManager : entityManagers_projectile) {
		entityManager->unloadMedia();
	}
	for (EntityManager * entityManager : entityManagers_nonprojectile) {
		entityManager->unloadMedia();
	}

	delete scoreTextBox;
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






ContainsBitmask App::rectContents(const SDL_Rect & r, const void * ignore) {
	ContainsBitmask contents = CONTAINS_NOTHING;

	if (r.x < 0 ||
			r.y < 0 ||
			r.x + r.w > gamescreen_width ||
			r.y + r.h > gamescreen_height)
		contents |= CONTAINS_OUTSIDE | CONTAINS_OBSTRUCTION;

	for (ProjectileList * projectileManager : entityManagers_projectile) {
		for (Projectile * projectile : projectileManager->projectiles)
			if (projectile->exploding && projectile->explode_frame < projectileManager->projectileTypeData.num_deadly_explosion_frames) {
				if (SDL_HasIntersection(&r,&(projectile->hitbox)) == SDL_TRUE)
					contents |= CONTAINS_DEADLY_EXPLOSION;
			}
			else if (!projectile->exploding)
				if (SDL_HasIntersection(&r,&(projectile->hitbox)) == SDL_TRUE && projectile!=ignore)
					contents |= CONTAINS_PROJECTILE;
	}


	for (Monster * monster : monster1List->monsters)
		if (SDL_HasIntersection(&r,&(monster->hitbox)) == SDL_TRUE)
			contents |= CONTAINS_OBSTRUCTION | CONTAINS_MONSTER | CONTAINS_MONSTER1;


	if (SDL_HasIntersection(&r,&(playerEntity->playerRect))==SDL_TRUE && playerEntity!=ignore)
		contents |= CONTAINS_OBSTRUCTION | CONTAINS_PLAYER;

	return contents;

}


void App::addScore(int s) {
	score += s;
	updateScoreTextBox();
}

void App::updateScoreTextBox() {
	std::string score_string = "Score: ";
	score_string += std::to_string(score);
	scoreTextBox->updateText(score_string.c_str());
}

void App::spawnMonster() {
	int monsterIndex = rand() % std::max(int(entityManagers_monster.size()),level-1);

	SDL_Rect spawnRect;
	spawnRect.w = entityManagers_monster[monsterIndex]->monsterTypeData.width;
	spawnRect.h = entityManagers_monster[monsterIndex]->monsterTypeData.height;
	bool acceptableSpawnRect = false;
	int tries = 0;

	while (!acceptableSpawnRect && tries < 200 ) {
			spawnRect.x = rand()% (gamescreen_width  - spawnRect.w ) ;
			spawnRect.y = rand()% (gamescreen_height - spawnRect.h ) ;
			if (!(rectContents(spawnRect) & CONTAINS_OBSTRUCTION )) {
				acceptableSpawnRect=true;
			}
			++tries;
	}
	if (acceptableSpawnRect)
		entityManagers_monster[monsterIndex]->createMonster(spawnRect.x,spawnRect.y);
}
