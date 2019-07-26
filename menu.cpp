#include "menu.h"
#include "main.h"

Menu::Menu(const char * title, Logger * log) : title(title), log(log) {

}

Menu::~Menu(){

}

void Menu::addItem(const char * itm_str, std::function<void ()> action){
  if (selected<0) selected=0;
  items.push_back(std::string(itm_str));
  actions.push_back(action);
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

void Menu::events(const SDL_Event * event){

}

void Menu::update(App * app){
  actions[0](); // temporary workaround until menu can render itself and respond to user input
}

void Menu::render(SDL_Renderer * renderer){

}
