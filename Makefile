
all: fire 
	

fire: main.cpp args.cpp args.hpp state.hpp state.cpp util.hpp
	g++ -std=c++0x -O3 -Wall -Wextra `libpng-config --cflags` `libpng-config --ldflags` args.cpp state.cpp main.cpp -o fire
