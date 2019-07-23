#include "textbox.h"

TextBox::TextBox(TTF_Font* font, SDL_Color* color, SDL_Renderer * renderer, Logger * log){

  if (color==nullptr || renderer==nullptr || font==nullptr)
    log->error("TextBox: Null pointer passed to constructor");

  this->color = color;
  this->renderer = renderer;
  this->font = font;
  this->log = log;

}

TextBox::~TextBox(){
  freeTexture();
}

TextBox::TextBox(TextBox && other) {
  color = other.color;
  renderer=other.renderer;
  font=other.font;
  log=other.log;
  textRect=other.textRect;

  mTexture = other.mTexture;
  other.mTexture = nullptr;
}

void TextBox::freeTexture(){
  if (mTexture!=nullptr){
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
  }
}

int TextBox::updateText(const char * text){

  if (color==nullptr  || renderer==nullptr || font==nullptr)
    log->error("TextBox: Some attribute has died before it should have");
  if (text==nullptr) log->error("TextBox: Null text has been passed");

  SDL_Surface * text_surface = TTF_RenderText_Blended(font, text, *color);
  if (text_surface==nullptr) {log->TTF_Error("Failed to render text"); return -1;}
  freeTexture();
  mTexture = SDL_CreateTextureFromSurface(renderer, text_surface);
  if (mTexture==nullptr) {log->SDL_Error("Failed to create textbox texture from surface"); return -1;}
  textRect.w = text_surface->w;
  textRect.h = text_surface->h;
  SDL_FreeSurface(text_surface);
  return 0;
}

int TextBox::renderCopy(int x, int y){
  textRect.x=x;
  textRect.y=y;
  if (mTexture==nullptr) {log->error("TextBox: renderCopy attempted with no text texture."); return -1;}
  return SDL_RenderCopy(renderer, mTexture, nullptr, &textRect);
}
