#pragma once

#include <SDL2/SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
 private:
  SDL_Window* window;
  SDL_Renderer* renderer;
  bool isRunning;
  int windowWidth;
  int windowHeight;
  int millisecsPreviousFrame;

  static const int GAME_WINDOW_WIDTH;
  static const int GAME_WINDOW_HEIGHT;

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
};
