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

	// if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"2") == SDL_FALSE) {
	// 	log.SDL_Error("Render scale quality hint not set.");
	// 	return -1;
	// }

	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
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


	// Due to some bug in SDL, Mix_OpenAudio needs to be called before Mix_Init
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,128)<0) {
		log.MIX_Error("Error opening audio in SDL_Mixer");
		return -1;
	}
	if (!(Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3)) {
		log.MIX_Error("Error initializing SDL_Mixer mp3 support");
		return -1;
	}
	Mix_AllocateChannels(16);



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


	//Load font
	font  = TTF_OpenFont("Roboto-Regular.ttf",16);
	if (font==nullptr){
		log.TTF_Error("Error loading font");
		return -1;
	}


	// --- Build up menus ---
	mainMenu = new Menu("Main Menu",renderer,font,&log);
	mainMenu->addItem("Start", [this] () {this->updateUIState(UI_STATE_GAME);});
	mainMenu->addItem("How to play", [this] () {this->updateUIState(UI_STATE_HOW_TO_PLAY);});
	mainMenu->addItem("Quit",  [this] () {this->updateUIState(UI_STATE_QUIT);});

	pauseMenu = new Menu("Pause Menu",renderer,font,&log);
	pauseMenu->addItem("Resume", [this] () {this->updateUIState(UI_STATE_GAME);});
	pauseMenu->addItem("How to play", [this] () {this->updateUIState(UI_STATE_HOW_TO_PLAY);});
	pauseMenu->addItem("Quit",   [this] () {this->updateUIState(UI_STATE_QUIT);});

	winMenu = new Menu("Win!  :D",renderer,font,&log);
	winMenu->addItem("Quit",   [this] () {this->updateUIState(UI_STATE_QUIT);});

	loseMenu = new Menu("Lost.  :(",renderer,font,&log);
	loseMenu->addItem("Quit",   [this] () {this->updateUIState(UI_STATE_QUIT);});

	help = new HelpScreen(renderer,font,&log);



	// --- Create EntityManager objects ---
	// This is where all monsters and projectiles are created.

	playerEntity = new PlayerEntity(); // will be freed when all of entityManagers items are freed in ~App()
	entityManagers_nonprojectile.push_back(playerEntity);

	ProjectileTypeData missile;
	missile.id=0;
	missile.num_frames = 2;
	missile.num_explosion_frames = 12;
	missile.num_deadly_explosion_frames = 4;
	missile.num_fadeout_frames = 8;
	missile.projectile_img_file = "missile.png";
	missile.explosion_img_file = "missile_explode.png";
	missile.projectile_launch_sound_file = "data/sounds/missile_launch.mp3";
	missile.projectile_die_sound_file = "data/sounds/placeholder.mp3";
	missile.projectile_img_frame_size = {16,49};
	missile.explosion_img_frame_size = {80,80};
	missile.width = 16;
	missile.height = 49;
	missile.explosion_width = 200;
	missile.explosion_height = 200;
	missile.projectile_hitbox = {0,0,16,30};
	missile.explosion_hitbox  = {26,25,33,33};
	missile.projectile_detonation_point = {8,5};
	missile.explosion_detonation_point = {38,39};
	missile.explosion_time_per_frame = 5;
	missile.explosion_only_harms_player = false;
	projectileList = new ProjectileList(missile);
	entityManagers_projectile.push_back(projectileList);

	ProjectileTypeData monster1bullets;
	monster1bullets.id=1;
	monster1bullets.num_frames = 2;
	monster1bullets.num_explosion_frames = 5;
	monster1bullets.num_deadly_explosion_frames = 1;
	monster1bullets.num_fadeout_frames = 4;
	monster1bullets.projectile_img_file = "monster1bullet.png";
	monster1bullets.explosion_img_file = "monster1bullet_explode.png";
	monster1bullets.projectile_launch_sound_file = "data/sounds/gem_launch.mp3";
	monster1bullets.projectile_die_sound_file = "data/sounds/placeholder.mp3";
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
	monster1bullets.explosion_only_harms_player = false;
	monster1bulletList = new ProjectileList(monster1bullets);
	entityManagers_projectile.push_back(monster1bulletList);

	MonsterTypeData monster1;
	monster1.name = "Gem";
	monster1.monster_img_file = "monster1.png";
	monster1.num_frames = 5;
	monster1.alive_time_per_frame = 10;
	monster1.num_death_frames = 9;
	monster1.num_fadeout_frames = 5;
	monster1.death_time_per_frame = 10;
	monster1.monster_img_frame_size = vecI(25,25);
	monster1.width  = 100;
	monster1.height = 100;
	monster1.hitbox = {10,11,3,3};
	monster1.projectile_launch_center = {11,12};
	monster1.projectile_launch_dist = 5;
	monster1.bulletManager = monster1bulletList;
	monster1.firePatternStr = "U:10;20 L:10;20 D:10;20 R:10;20";
	monster1.protected_by = CONTAINS_IMPOSSIBLE;
	monster1List = new MonsterList(monster1,&log);
	entityManagers_nonprojectile.push_back(monster1List);
	entityManagers_monster.push_back(monster1List);

	ProjectileTypeData monster2bullets;
	monster2bullets.id=2;
	monster2bullets.num_frames = 6;
	monster2bullets.num_explosion_frames = 6;
	monster2bullets.num_deadly_explosion_frames = 3;
	monster2bullets.num_fadeout_frames = 4;
	monster2bullets.projectile_img_file = "monster2bullet.png";
	monster2bullets.explosion_img_file = "monster2bullet_explode.png";
	monster2bullets.projectile_launch_sound_file = "data/sounds/placeholder.mp3";
	monster2bullets.projectile_die_sound_file = "data/sounds/placeholder.mp3";
	monster2bullets.projectile_img_frame_size = {80,80};
	monster2bullets.explosion_img_frame_size = {80,80};
	monster2bullets.width = 150;
	monster2bullets.height = 100;
	monster2bullets.explosion_width = 150;
	monster2bullets.explosion_height = 100;
	monster2bullets.projectile_hitbox = {13,29,53,21};
	monster2bullets.explosion_hitbox  = {13,29,53,21};
	monster2bullets.projectile_detonation_point = {0,0};
	monster2bullets.explosion_detonation_point = {0,0};
	monster2bullets.explosion_time_per_frame = 5;
	monster2bullets.explosion_only_harms_player = false;
	monster2bulletList = new ProjectileList(monster2bullets);
	entityManagers_projectile.push_back(monster2bulletList);

	MonsterTypeData monster2;
	monster2.name = "Octo";
	monster2.monster_img_file = "monster2.png";
	monster2.num_frames = 4;
	monster2.alive_time_per_frame = 10;
	monster2.num_death_frames = 6;
	monster2.num_fadeout_frames = 5;
	monster2.death_time_per_frame = 10;
	monster2.monster_img_frame_size = vecI(80,80);
	monster2.width  = 100;
	monster2.height = 100;
	monster2.hitbox = {31,30,16,18};
	monster2.projectile_launch_center = {38,39};
	monster2.projectile_launch_dist = 0;
	monster2.bulletManager = monster2bulletList;
	monster2.firePatternStr = "D:3;35 U:3;35 R:3;35 L:3;35 U:3;35 D:3;35 L:3;35 R:3;35";
	monster2.protected_by = CONTAINS_IMPOSSIBLE;
	monster2List = new MonsterList(monster2,&log);
	entityManagers_nonprojectile.push_back(monster2List);
	entityManagers_monster.push_back(monster2List);

	ProjectileTypeData monster3bullets;
	monster3bullets.id=3;
	monster3bullets.num_frames = 7;
	monster3bullets.num_explosion_frames = 14;
	monster3bullets.num_deadly_explosion_frames = 12;
	monster3bullets.num_fadeout_frames = 7;
	monster3bullets.projectile_img_file = "monster3bullet.png";
	monster3bullets.explosion_img_file = "monster3bullet_explode.png";
	monster3bullets.projectile_launch_sound_file = "data/sounds/placeholder.mp3";
	monster3bullets.projectile_die_sound_file = "data/sounds/placeholder.mp3";
	monster3bullets.projectile_img_frame_size = {9,9};
	monster3bullets.explosion_img_frame_size = {100,100};
	monster3bullets.width = 9;
	monster3bullets.height = 9;
	monster3bullets.explosion_width = 400;
	monster3bullets.explosion_height = 400;
	monster3bullets.projectile_hitbox = {2,2,5,5};
	monster3bullets.explosion_hitbox  = {16,16,73,67};
	monster3bullets.projectile_detonation_point = {4,4};
	monster3bullets.explosion_detonation_point = {50,52};
	monster3bullets.explosion_time_per_frame = 5;
	monster3bullets.explosion_only_harms_player = true;
	monster3bulletList = new ProjectileList(monster3bullets);
	entityManagers_projectile.push_back(monster3bulletList);

	MonsterTypeData monster3;
	monster3.name = "Mushy";
	monster3.monster_img_file = "monster3.png";
	monster3.num_frames = 4;
	monster3.alive_time_per_frame = 10;
	monster3.num_death_frames = 6;
	monster3.num_fadeout_frames = 5;
	monster3.death_time_per_frame = 6;
	monster3.monster_img_frame_size = vecI(80,80);
	monster3.width  = 80;
	monster3.height = 80;
	monster3.hitbox = {19,14,44,49};
	monster3.projectile_launch_center = {39,38};
	monster3.projectile_launch_dist = 29;
	monster3.bulletManager = monster3bulletList;
	monster3.firePatternStr = "R:10,L:10;100 U:10,D:10;100";
	monster3.protected_by = CONTAINS_IMPOSSIBLE;
	monster3List = new MonsterList(monster3,&log);
	entityManagers_nonprojectile.push_back(monster3List);
	entityManagers_monster.push_back(monster3List);





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
	Mix_Quit();
	Mix_CloseAudio();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}


int App::execute(){

	while (ui_state!=UI_STATE_QUIT){

		Uint32 t0 = SDL_GetTicks();

		SDL_Event event;
		Menu * endMenu = won ? winMenu : loseMenu;

		switch (ui_state) {
			case UI_STATE_MENU:
				while(SDL_PollEvent(&event)) {
					appEvent(&event);
					mainMenu->events(&event);
				}
				mainMenu->update(this);
				mainMenu->render(renderer);
				break;
			case UI_STATE_GAME:
				while(SDL_PollEvent(&event)) {
					appEvent(&event);
					gameEvent(&event);
				}
				gameUpdate();
				gameRender();
				break;
			case UI_STATE_ENDGAME:
				while(SDL_PollEvent(&event)) {
					appEvent(&event);
					endMenu->events(&event);
				}
				endMenu->update(this);
				endMenu->render(renderer);
				break;
			case UI_STATE_PAUSE:
				while(SDL_PollEvent(&event)) {
					appEvent(&event);
					pauseMenu->events(&event);
				}
				pauseMenu->update(this);
				pauseMenu->render(renderer);
				break;
			case UI_STATE_HOW_TO_PLAY:
				while(SDL_PollEvent(&event)) {
					appEvent(&event);
					help->events(&event);
				}
				help->update(this);
				help->render(renderer);
				break;
			default:
				while(SDL_PollEvent(&event)) {
					appEvent(&event);
				}
				break;
		}

		Uint32 dt = SDL_GetTicks()-t0;
		SDL_Delay( dt<20 ? 20-dt : 0 );
	}

	return 0;
}


void App::appEvent(SDL_Event * event) {

		switch (event->type) {
			case SDL_QUIT:
				ui_state=UI_STATE_QUIT;
				break;
			case SDL_KEYDOWN:
				handleKeypress(&(event->key));
				break;
		}
}

void App::gameEvent(SDL_Event * event){

	for (EntityManager * entityManager : entityManagers_projectile)
		entityManager->handleEvent(event);
	for (EntityManager * entityManager : entityManagers_nonprojectile)
		entityManager->handleEvent(event);

}


void App::gameUpdate(){

	// Let all entity managers do their updates
	for (EntityManager * entityManager : entityManagers_projectile)
		entityManager->update(this);
	for (EntityManager * entityManager : entityManagers_nonprojectile)
		entityManager->update(this);

	// Update monster spawn wave countdown and spawn monsters if it's time
	if (wave_countdown>0) --wave_countdown;
	else {
		const int score_to_win = SCORE_PER_LEVEL_ADVANCE * num_levels;
		const int wave_time_when_score_zero = 400;
		const int wave_time_when_win = 130;
		int wave_time_base = score * ((wave_time_when_win - wave_time_when_score_zero)/score_to_win)
														   + wave_time_when_score_zero;
		if (wave_time_base < wave_time_when_win) wave_time_base = wave_time_when_win;
		// Condition in this if should not happen before win, but it's good to prevent possible % 0 below.
		wave_countdown = wave_time_base + (rand() % (wave_time_base/2));
		spawnWave();
	}

	// If there are currently no monsters, then tick down the wave countdown even further
	if (wave_countdown>0 && noMonsters()) --wave_countdown;


	// Check if level increases or win state changes
	if (score >= level * SCORE_PER_LEVEL_ADVANCE) ++level;
	if (level > num_levels) win();

	// Update status indicators
	missileLoadingIndicator->setProgress( float(playerEntity->missile_cooldown - playerEntity->missile_cooldown_countdown)/float(playerEntity->missile_cooldown) );
	shieldIndicator->setProgress( float(playerEntity->shield_recharge_time - playerEntity->shield_timer)/float(playerEntity->shield_recharge_time) );



}


void App::gameRender(){

	// Set the renderer's viewport to the right hand menu
	SDL_RenderSetViewport(renderer,&info_area_rect);

	SDL_SetRenderDrawColor(renderer,100,100,50,255);
	SDL_RenderFillRect(renderer,nullptr);

	int x = 10; int y = 10;
	scoreTextBox->renderCopy(x,y);

	x += 120;
	std::string fpsString = "FPS: ";
	fpsString += std::to_string(fps);
	if (frame % 10 ==0 ) fpsTextBox->updateText(fpsString.c_str());
	// fpsTextBox->updateText((std::to_string(wave_countdown)).c_str()); // Putting random things here is a good way to test stuff sometimes
	fpsTextBox->renderCopy(x,y);

	x += 100;
	missileLoadingIndicator->render(x,y);

	x += 130;
	shieldIndicator->render(x,y);

	x += 150; y += 10;
	renderScoreIndicator(x,y);

	// Set the renderer's viewport to the game screen
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



	// Initialize text Textboxes
	scoreTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	updateScoreTextBox();
	fpsTextBox = new TextBox(font, &(palette[PALETTE_BLACK]), renderer, &log);
	fpsTextBox->updateText("[FPS]");

	// Create statusIndicators
	missileLoadingIndicator = createStatusIndicator(120,25,"Missile ready","Reloading");
	shieldIndicator = createStatusIndicator(120,25,"Shield on","Recharging");





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
	delete missileLoadingIndicator;
	delete shieldIndicator;

	delete mainMenu; mainMenu = nullptr;
	delete pauseMenu; pauseMenu = nullptr;
	delete winMenu; winMenu = nullptr;
	delete loseMenu; loseMenu = nullptr;
	delete help; help = nullptr;

	TTF_CloseFont(font);
	font = nullptr;

}

void App::handleKeypress(SDL_KeyboardEvent * key){

	switch (key->keysym.scancode) {
		case SDL_SCANCODE_SPACE:
			timerStart = SDL_GetTicks();
			break;
		case SDL_SCANCODE_Q:
			ui_state=UI_STATE_QUIT;
			break;
		case SDL_SCANCODE_ESCAPE:
			if (ui_state==UI_STATE_GAME) ui_state=UI_STATE_PAUSE;
			else if (ui_state==UI_STATE_PAUSE) ui_state=UI_STATE_GAME;
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
				if (SDL_HasIntersection(&r,&(projectile->hitbox)) == SDL_TRUE){
					if (projectileManager->projectileTypeData.explosion_only_harms_player)
						contents |= CONTAINS_DEADLY_TO_PLAYER;
					else
						contents |= CONTAINS_DEADLY_EXPLOSION;
					if (projectileManager->projectileTypeData.id == 3)
						contents |= CONTAINS_SPORES;
					if (projectile->launched_by_player)
						contents |= CONTAINS_PLAYER_CAUSED_EXPLOSION;
				}
			}
			else if (!projectile->exploding)
				if (SDL_HasIntersection(&r,&(projectile->hitbox)) == SDL_TRUE && projectile!=ignore)
					contents |= CONTAINS_PROJECTILE;
	}


	for (MonsterList * monsterManager : entityManagers_monster)
		for (Monster * monster : monsterManager->monsters)
			if (SDL_HasIntersection(&r,&(monster->hitbox)) == SDL_TRUE)
				contents |= CONTAINS_OBSTRUCTION | CONTAINS_MONSTER;


	if (SDL_HasIntersection(&r,&(playerEntity->playerHitbox))==SDL_TRUE && playerEntity!=ignore)
		contents |= CONTAINS_OBSTRUCTION | CONTAINS_PLAYER;

	return contents;

}


void App::addScore(int s) {
	score += s;
	if (score < 0) score=0;
	updateScoreTextBox();
}

void App::updateScoreTextBox() {
	std::string score_string = "Score: ";
	score_string += std::to_string(score);
	scoreTextBox->updateText(score_string.c_str());
}

StatusIndicator * App::createStatusIndicator(int width, int height, const char * green_msg, const char * red_msg){
	return new StatusIndicator(width, height,green_msg,red_msg,
														 font, &(palette[PALETTE_BLACK]), renderer, &log);
}

void App::renderScoreIndicator(int x, int y) {
	const int indicator_width  = 200;
	const int back_bar_height  = 5;
	const int slider_width = 6;
	const int slider_height = 15;
	const int back_bar_y_offset = (slider_height/2) - (back_bar_height/2);
	const int score_to_win = SCORE_PER_LEVEL_ADVANCE * num_levels;
	const float distance_per_score =  float(indicator_width) / float(score_to_win)  ;
	const int back_bar_pos_width = int(distance_per_score*(score_to_win));
	const int slider_center_dist = int(distance_per_score*score);
	const int slider_x_offset = slider_center_dist-(slider_width/2);

	const SDL_Rect back_bar_pos = {x,y+back_bar_y_offset,back_bar_pos_width,back_bar_height};
 	SDL_SetRenderDrawColor(renderer,0,180,0,255);
	SDL_RenderFillRect(renderer,&back_bar_pos);

	const SDL_Rect slider_rect = {x+slider_x_offset,y,slider_width,slider_height};
 	SDL_SetRenderDrawColor(renderer,50,50,50,255);
	SDL_RenderFillRect(renderer,&slider_rect);
}


void App::spawnMonster() {
	int monsterIndex = rand() % std::min(int(entityManagers_monster.size()),level);

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

void App::spawnWave() {
	int wave_size = (rand() % (WAVESIZE_MAX-WAVESIZE_MIN + 1)) + WAVESIZE_MIN;
	for (int i = 0; i < wave_size; i++)
		spawnMonster();
}


void App::updateUIState(UI_State new_state) {
	previous_ui_state = ui_state;
	ui_state = new_state;
}

void App::revertUIState() {
	ui_state = previous_ui_state;
}

bool App::noMonsters() {
	for (MonsterList * monsterManager : entityManagers_monster) {
		if (!(monsterManager->monsters.empty())) return false;
	}
	return true;
}
