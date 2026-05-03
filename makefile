all: default

default:
	g++ main.cpp -o main -I/usr/include/freetype2 -lGLEW -lglfw -lGL -ldl -lsoil2 -lfreetype -g
