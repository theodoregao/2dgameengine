CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
DEBUG_COMPILER_FLAGS = $(COMPILER_FLAGS) -g
INCLUDE_PATH = -I"./libs/"
SRC_FILES = src/*.cpp \
			src/AssetStore/*.cpp \
			src/ECS/*.cpp \
			src/Game/*.cpp \
			src/Logger/*.cpp
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3
OBJ_NAME = out/gameengine
DEBUG_OBJ_NAME = out/gameengine-debug
BUILD_COMMAND = $(CC) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS)

build:
	$(BUILD_COMMAND) $(COMPILER_FLAGS) -o $(OBJ_NAME)

debug:
	$(BUILD_COMMAND) $(DEBUG_COMPILER_FLAGS) -o $(DEBUG_OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME) $(DEBUG_OBJ_NAME)
