#include "status_indicator.h"

StatusIndicator::StatusIndicator(int width, int height, const char * green_message, const char * red_message,
                                 TTF_Font* font, SDL_Color* color, SDL_Renderer * renderer, Logger * log)
                                : width(width),
                                  height(height),
                                  renderer(renderer),
                                  log(log)
{
  greenText = new TextBox(font,color,renderer,log);
  redText = new TextBox(font,color,renderer,log);
  greenText->updateText(green_message);
  redText->updateText(red_message);
}

StatusIndicator::~StatusIndicator() {
  delete greenText;
  delete redText;
}

void StatusIndicator::render(int x, int y) {
  SDL_Rect target_rect = {x,y,width,height};
  if (green) SDL_SetRenderDrawColor(renderer, 0,255,0,255);
  else       SDL_SetRenderDrawColor(renderer, 255,0,0,255);
  SDL_RenderFillRect(renderer,&target_rect);
  if (green) greenText->renderCopy(x+2,y); // create bit of left margin there
  else       redText->renderCopy(x+2,y);
}

void StatusIndicator::setGreen() {
  green=true;
}

void StatusIndicator::setRed() {
  green=false;
}
