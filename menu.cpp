#include "menu.h"
#include "main.h"
#include "utilities.h"

Menu::Menu(const char * title,
           SDL_Renderer * renderer,
           TTF_Font * font,
           Logger * log) : title(title),
                           renderer(renderer),
                           font(font),
                           log(log) {
  textColor = {0,0,0,255};

  SDL_Surface * surf = TTF_RenderText_Blended(font,this->title.c_str(),textColor);
  titleTextTexture = SDL_CreateTextureFromSurface(renderer,surf);
  SDL_FreeSurface(surf);
  if (titleTextTexture==nullptr) {
    std::string error_msg = "Could not create text texture for title of menu ";
    error_msg += title;
    log->SDL_Error(error_msg.c_str());
  }
}

Menu::~Menu(){
  for (SDL_Texture * texture : itemTextTextures) {
    SDL_DestroyTexture(texture);
  }
  SDL_DestroyTexture(titleTextTexture);
}

void Menu::addItem(const char * itm_str, std::function<void ()> action){
  if (selected<0) selected=0;
  items.push_back(std::string(itm_str));
  actions.push_back(action);

  SDL_Surface * surf = TTF_RenderText_Blended(font,itm_str,textColor);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer,surf);
  SDL_FreeSurface(surf);
  if (texture==nullptr) {
    std::string error_msg = "Could not create text texture for menu item \"";
    error_msg += itm_str;
    error_msg += "\" in menu \"";
    error_msg += title;
    error_msg += "\".";
    log->SDL_Error(error_msg.c_str());
  }

  itemTextTextures.push_back(texture);
}

void Menu::nextOption() {
  if (selected<0) {
    std::string error_msg = "Attempted nextOption in menu \"";
    error_msg += title;
    error_msg += "\" which has no options.";
    log->error(error_msg.c_str());
  }
  selected = (selected + 1) % int(items.size());
}

void Menu::prevOption() {
  if (selected<0) {
    std::string error_msg = "Attempted prevOption in menu \"";
    error_msg += title;
    error_msg += "\" which has no options.";
    log->error(error_msg.c_str());
  }
  selected = (selected>0)
             ? (selected - 1)
             : (int(items.size())-1);
}


void Menu::events(const SDL_Event * event){
  if (event->type==SDL_KEYDOWN) {
    switch(event->key.keysym.scancode) {
      case SDL_SCANCODE_UP:
        prevOption();
        break;
      case SDL_SCANCODE_DOWN:
        nextOption();
        break;
      case SDL_SCANCODE_KP_ENTER:
      case SDL_SCANCODE_RETURN:
      case SDL_SCANCODE_SPACE:
        actions[selected]();
        break;
      default: break;
    }
  }
}

void Menu::update(App * app){
  // actions[0](); // temporary workaround until menu can render itself and respond to user input
}

void Menu::render(SDL_Renderer * renderer){

  if (font==nullptr) { // If we forgot to setFont
    std::string error_msg = "Attempting to render menu \"";
    error_msg += title;
    error_msg += "\" but a font was never set for this menu!";
    log->error(error_msg.c_str());
    return;
  }

  SDL_RenderSetViewport(renderer,nullptr); // Set viewport to entire window

  SDL_SetRenderDrawColor(renderer,100,100,100,255);
  SDL_RenderFillRect(renderer,nullptr);

  int lineskip = TTF_FontLineSkip(font);

  int x = 150; // Just whatever. Not trying to make best UI framework ever here.
  int y = 150;

  int w,h;
  SDL_QueryTexture(titleTextTexture,nullptr,nullptr,&w,&h);
  SDL_Rect target_rect = {x,y,int(1.5*w),int(1.5*h)};
  SDL_RenderCopy(renderer,titleTextTexture,nullptr,&target_rect);

  y += int(1.5*lineskip)+30;
  x += 20;

  for (unsigned int i = 0; i < items.size(); i++) {
    SDL_QueryTexture(itemTextTextures[i],nullptr,nullptr,&w,&h);
    target_rect = {x,y,w,h};
    SDL_RenderCopy(renderer,itemTextTextures[i],nullptr,&target_rect);
    if (i==(unsigned int)(selected)) {
      SDL_SetRenderDrawColor(renderer,0,200,200,255);
      SDL_Point arrow [] = {{x-20,y+h/2},
                            {x-5,y+h/2},
                            {x-7,y+h/4},
                            {x-7,y+3*h/4},
                            {x-5,y+h/2}};
      SDL_RenderDrawLines(renderer,arrow,5);
    }
    y += lineskip;
  }




  SDL_RenderPresent( renderer );
}
