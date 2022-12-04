build:
	g++ \
	-Wall \
	-std=c++17 \
	-I"./libs/" \
	src/*.cpp \
	-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
	-llua5.3 \
	-o out/gameengine

run:
	out/gameengine

clean:
	rm out/*
