pote: src/main.cpp
	mkdir -p bin/
	g++ --std=c++11 -mconsole -o bin/toyunda_player src/main.cpp -I/mingw64/include/ -L/mingw64/lib/ -lmpv -lsdl2 -lsdl2_ttf -lopengl32 
	# -mconsole
