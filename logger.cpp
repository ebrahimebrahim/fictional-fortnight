#include "logger.h"

Logger::Logger(){

}

Logger::~Logger(){

}

void Logger::error(const char * msg){
  printf( "Error: %s\n", msg );
}

void Logger::SDL_Error(const char * msg){
  std::string msg_str = msg;
  msg_str += " ; here's the latest SDL error: ";
  msg_str += SDL_GetError();
  error(msg_str.c_str());
}

void Logger::IMG_Error(const char * msg){
  std::string msg_str = msg;
  msg_str += " ; here's the latest IMG error: ";
  msg_str += IMG_GetError();
  error(msg_str.c_str());
}

void Logger::TTF_Error(const char * msg){
  std::string msg_str = msg;
  msg_str += " ; here's the latest TTF error: ";
  msg_str += TTF_GetError();
  error(msg_str.c_str());
}
