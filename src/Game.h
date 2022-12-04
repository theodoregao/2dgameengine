#pragma once

#include <SDL2/SDL.h>

class Game {
 private:
  SDL_Window* window;
  SDL_Renderer* renderer;
  bool isRunning;
  int windowWidth;
  int windowHeight;

  static const int GAME_WINDOW_WIDTH;
  static const int GAME_WINDOW_HEIGHT;

 public:
  Game();
  ~Game();
  void Initialize();
  void Run();
  void ProcessInput();
  void Update();
  void Render();
  void Destory();
};
