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
  // draw red rect
  SDL_SetRenderDrawColor(renderer, 255,0,0,255);
  SDL_Rect target_rect = {x,y,width,height};
  SDL_RenderFillRect(renderer,&target_rect);

  // draw green rect
  SDL_SetRenderDrawColor(renderer, 0,255,0,255);
  target_rect = {x,y,int(float(width)*progress),height};
  SDL_RenderFillRect(renderer,&target_rect);

  if (progress>0.99) greenText->renderCopy(x+2,y); // create bit of left margin there
  else       redText->renderCopy(x+2,y);
}

void StatusIndicator::setProgress(float progress) {
  this->progress=progress;
}
