pote: src/main.cpp
	mkdir -p bin/
	g++ -o bin/toyunda_player -mconsole src/main.cpp -I/mingw64/include/ -L/mingw64/lib/ -lmpv -lsdl2
