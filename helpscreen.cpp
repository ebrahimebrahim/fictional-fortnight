#include "main.h"
#include "helpscreen.h"

HelpScreen::HelpScreen(SDL_Renderer * renderer, TTF_Font * font, Logger * log) :
  renderer(renderer),
  font(font),
  log(log),
  textColor{0,0,0,255},
  helpTextBox(font,&textColor,renderer,log)
{
  helpTextBox.updateText("Try to win. Press any key to return.");
}



void HelpScreen::events(const SDL_Event * event) {
  if (event->type==SDL_KEYDOWN) {
    tryExitHelp = true;
  }
}

void HelpScreen::update(App * app) {
  if (tryExitHelp) {
    tryExitHelp = false;
    app->revertUIState();
  }
}

void HelpScreen::render(SDL_Renderer * renderer) {
  SDL_SetRenderDrawColor(renderer,100,100,100,255);
  SDL_RenderFillRect(renderer,nullptr);
  helpTextBox.renderCopy(150,150);
  
  SDL_RenderPresent( renderer );
}
