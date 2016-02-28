pote: src/main.cpp
	mkdir -p bin/
	g++ -o bin/toyunda_player $^ `pkg-config --libs --cflags mpv sdl2`
