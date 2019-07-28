#include "textbox.h"
#include "utilities.h"
#include <algorithm>

TextBox::TextBox(TTF_Font* font, SDL_Color* color, SDL_Renderer * renderer, Logger * log){

  if (color==nullptr || renderer==nullptr || font==nullptr)
    log->error("TextBox: Null pointer passed to constructor");

  this->color = color;
  this->renderer = renderer;
  this->font = font;
  this->log = log;
  this->width = width;
  this->lineskip = TTF_FontLineSkip(font);

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

void TextBox::freeTextures(std::vector<SDL_Texture *> & textures) {
  for (SDL_Texture * t : textures) {
    if (t!=nullptr) {
      SDL_DestroyTexture(t);
      t=nullptr;
    }
  }
}

SDL_Texture * TextBox::createTexture(const char * text, int * w, int * h) {
  SDL_Surface * text_surface = TTF_RenderText_Blended(font, text, *color);
  if (text_surface==nullptr) {log->TTF_Error("Failed to render text"); return nullptr;}
  if (w!=nullptr) *w = text_surface->w;
  if (h!=nullptr) *h = text_surface->h;
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

  std::vector<SDL_Texture *> line_textures;
  std::vector<int> line_widths;
  int w=0;
  for (std::string line_till_newline : split(text,"\n")) {
    // TODO do further split of line_by_newline for wrapping
    SDL_Texture * line_texture = createTexture(line_till_newline.c_str(), &w, nullptr);
    if (line_texture == nullptr) {
      freeTextures(line_textures);
      return -1;
    }
    line_textures.push_back(line_texture);
    line_widths.push_back(w);
  }

  textRect.h = lineskip * int(line_textures.size()); // This is now nonzero iff line_textures is nonempty.
  textRect.w = *(std::max_element(line_widths.begin(),line_widths.end())); //TODO remove -10.

  if (textRect.h != 0) { // If it's 0 then this signals that we are holding empty text, which is fine.
    Uint32 texture_format;
    SDL_QueryTexture(line_textures[0],&texture_format,nullptr,nullptr,nullptr);
    mTexture = SDL_CreateTexture(renderer,texture_format,SDL_TEXTUREACCESS_TARGET,textRect.w,textRect.h);
    if (mTexture==nullptr) {log->SDL_Error("TextBox: Could not create texture");}
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, mTexture);

    for (int i = 0; i<int(line_textures.size()); i++) {
      SDL_Rect line_rect = {0,i*lineskip,line_widths[i],lineskip};
      SDL_RenderCopy(renderer,line_textures[i],nullptr,&line_rect);
    }

    SDL_SetRenderTarget(renderer, nullptr);
  }

  freeTextures(line_textures);

  return 0;
}

int TextBox::renderCopy(int x, int y){
  if (textRect.h==0) return 0; // This means we have nothing to render, no problem.
  textRect.x=x;
  textRect.y=y;
  if (mTexture==nullptr) {log->error("TextBox: renderCopy attempted with no text texture."); return -1;}
  return SDL_RenderCopy(renderer, mTexture, nullptr, &textRect);
}
