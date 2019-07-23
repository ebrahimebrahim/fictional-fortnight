#include "textbox.h"
#include "utilities.h"

class StatusIndicator {
  public:
    StatusIndicator(int width, int height, const char * green_message, const char * red_message,
                    TTF_Font* font, SDL_Color* color, SDL_Renderer * renderer, Logger * log);
    ~StatusIndicator();

    void render(int x, int y);
    void setGreen();
    void setRed();

  private:
    TextBox * greenText = nullptr;
    TextBox * redText = nullptr;
    int width;
    int height;
    bool green = false;
    SDL_Renderer * renderer = nullptr;
    Logger * log = nullptr;
};
