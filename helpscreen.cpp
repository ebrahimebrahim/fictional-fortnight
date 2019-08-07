#include "main.h"
#include "helpscreen.h"
#include <string>
#include <sstream>
#include <fstream>

HelpScreen::HelpScreen(SDL_Renderer * renderer, TTF_Font * font, Logger * log) :
  renderer(renderer),
  font(font),
  log(log),
  textColor{0,0,0,255},
  helpTextBox(font,&textColor,renderer,log)
{
  helpTextBox.width=600;
  std::string helpFileName = "helptext.txt";
  std::ifstream ifs(helpFileName.c_str());
  if (!ifs) {
    log->error((helpFileName+" could not be loaded.").c_str());
    helpTextBox.updateText("Help text was not loaded.");
  }
  else {
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string helpText = ss.str();
    if (helpTextBox.updateText(helpText.c_str()) < 0) log->error("problem drawing help text");
  }
  ifs.close();
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

  // These are for testing.
  // SDL_SetRenderDrawColor(renderer,255,100,100,255);
  // SDL_Rect r = {150,150,helpTextBox.width,300};
  // SDL_RenderDrawRect(renderer,&r);

}
