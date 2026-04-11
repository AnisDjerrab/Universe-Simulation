all: default

default:
	g++ main.cpp -o main -lGLEW -lglfw -lGL -ldl -lsoil2 -g
	
