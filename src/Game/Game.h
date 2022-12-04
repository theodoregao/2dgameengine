#pragma once

#include <SDL2/SDL.h>

#include <memory>

#include "../ECS/ECS.h"

const int GAME_WINDOW_WIDTH = 800;
const int GAME_WINDOW_HEIGHT = 600;
const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
 public:
  Game();
  ~Game();
  void Initialize();
  void Run();
  void Setup();
  void ProcessInput();
  void Update();
  void Render();
  void Destory();

 private:
  SDL_Window* window;
  SDL_Renderer* renderer;
  std::unique_ptr<Registry> registry;
  bool isRunning;
  int windowWidth;
  int windowHeight;
  int millisecsPreviousFrame;
};
