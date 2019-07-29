#include "textbox.h"
#include "utilities.h"
#include <algorithm>
#include <iostream> // TODO DELETE THIS WHEN DONE TESTING

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
    if (width<0) { // No line wrapping
      SDL_Texture * line_texture = createTexture(line_till_newline.c_str(), &w, nullptr);
      if (line_texture == nullptr) {
        freeTextures(line_textures);
        return -1;
      }
      line_textures.push_back(line_texture);
      line_widths.push_back(w);
    }
    else { // If line wrapping enabled
      SDL_Texture * line_texture = nullptr;
      int towrap_len = line_till_newline.length();
      char * towrap = new char [towrap_len];
      strcpy(towrap,line_till_newline.c_str());
      char * head = towrap;
      int end_index = towrap_len; // throughout loop we will maintain that head[end_index] is the very last null character
      char swapchar;
      int i;
      int previous_cut_index;

      while (end_index>0) {
        previous_cut_index = 0;
        for (i=0; i <= end_index; ++i) {
          if (head[i]==' ' || i==end_index) {
            swapchar = head[i];
            head[i]='\0';
            TTF_SizeText(font,head,&w,nullptr);
            head[i]=swapchar;
            if (w>width) break;
            previous_cut_index=i;
          }
        }
        if (i==end_index+1) { // If the whole thing fits
          line_texture = createTexture(head, &w, nullptr);
          end_index = 0;
        }
        else if (previous_cut_index>0) { // If only part of it fits
          head[previous_cut_index]='\0';
          line_texture = createTexture(head, &w, nullptr);
          head += previous_cut_index+1;
          end_index -= previous_cut_index+1;
        }
        else { // If not even the first word fits
          for(i=1; i<end_index; i++) {
            swapchar = head[i];
            head[i]='\0';
            TTF_SizeText(font,head,&w,nullptr);
            head[i]=swapchar;
            if (w>width) break;
          }
          i = std::max(1,i-1); // Now i is cut index. If even first letter didn't fit, we give up and let i=1 anyway, allowing overflow.
          swapchar = head[i];
          head[i]='\0';
          line_texture = createTexture(head, &w, nullptr);
          head[i]=swapchar;
          head += i;
          end_index -= i;
        }

        if (line_texture == nullptr) {
          freeTextures(line_textures);
          return -1;
        }
        line_textures.push_back(line_texture);
        line_widths.push_back(w);

      }

      delete [] towrap;
    }
  }

  textRect.h = lineskip * int(line_textures.size()); // This is now nonzero iff line_textures is nonempty.

  if (textRect.h != 0) { // If it's 0 then this signals that we are holding empty text, which is fine.
    textRect.w = *(std::max_element(line_widths.begin(),line_widths.end()));
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
