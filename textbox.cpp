#include "textbox.h"

TextBox::TextBox(TTF_Font* font, SDL_Color* color, SDL_Renderer * renderer, Logger * log, int width){

  if (color==nullptr || renderer==nullptr || font==nullptr)
    log->error("TextBox: Null pointer passed to constructor");

  this->color = color;
  this->renderer = renderer;
  this->font = font;
  this->log = log;
  this->width = width;

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

SDL_Texture * TextBox::createTexture(const char * text, int * w, int * h) {
  SDL_Surface * text_surface = TTF_RenderText_Blended(font, text, *color);
  if (text_surface==nullptr) {log->TTF_Error("Failed to render text"); return nullptr;}
  *w = text_surface->w;
  *h = text_surface->h;
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, text_surface);
  SDL_FreeSurface(text_surface);
  if (texture==nullptr) {log->SDL_Error("Failed to create textbox texture from surface"); return nullptr;}
  return texture;
}

int TextBox::updateText(const char * text){

  if (color==nullptr  || renderer==nullptr || font==nullptr)
    log->error("TextBox: Some attribute has died before it should have");
  if (text==nullptr) log->error("TextBox: Null text has been passed");

  freeTexture();
  mTexture = createTexture(text,&(textRect.w),&(textRect.h));
  if (mTexture == nullptr) return -1;
  return 0;
}

int TextBox::renderCopy(int x, int y){
  textRect.x=x;
  textRect.y=y;
  if (mTexture==nullptr) {log->error("TextBox: renderCopy attempted with no text texture."); return -1;}
  return SDL_RenderCopy(renderer, mTexture, nullptr, &textRect);
}
