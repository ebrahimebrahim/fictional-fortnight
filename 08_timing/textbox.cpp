#include "textbox.h"

TextBox::TextBox(TTF_Font* font, SDL_Color* color, SDL_Renderer * renderer, Logger * log){

  if (color==nullptr || renderer==nullptr || font==nullptr){
    log->error("TextBox: Null pointer passed to constructor");
  }

  this->color = color;
  this->renderer = renderer;
  this->font = font;

}

TextBox::~TextBox(){
  freeTexture();
}

void TextBox::freeTexture(){
  if (mTexture!=nullptr)
    SDL_DestroyTexture(mTexture);
}

void TextBox::updateText(const char * text){
  freeTexture();
  SDL_Surface * text_surface = TTF_RenderText_Blended(font, text, *color);
  mTexture = SDL_CreateTextureFromSurface(renderer, text_surface);
  textRect.w = text_surface->w;
  textRect.h = text_surface->h;
  SDL_FreeSurface(text_surface);
}

void TextBox::renderCopy(int x, int y){
  textRect.x=x;
  textRect.y=y;
  SDL_RenderCopy(renderer, mTexture, nullptr, &textRect);
}
