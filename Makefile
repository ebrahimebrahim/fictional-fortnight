OBJS = main.cpp logger.cpp textbox.cpp playerentity.cpp utilities.cpp projectile.cpp monster.cpp firePatternParser.cpp status_indicator.cpp menu.cpp helpscreen.cpp

OBJ_NAME = peup.out

all : $(OBJS)
	g++ $(OBJS) -Wall -std=c++17 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o $(OBJ_NAME)
