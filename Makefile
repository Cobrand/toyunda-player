pote: src/main.cpp
	mkdir -p bin/
	g++ -std=c++11 -o bin/toyunda_player $^ `pkg-config --libs --cflags mpv sdl2 SDL2_ttf gl`
