#pragma once

#include <SDL2/SDL.h>

#include <memory>

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
 public:
  static int windowWidth;
  static int windowHeight;
  static int mapWidth;
  static int mapHeight;

 public:
  Game();
  ~Game();
  void Initialize();
  void Run();
  void Setup();
  void LoadLevel(int level);
  void ProcessInput();
  void Update();
  void Render();
  void Destory();

 private:
  SDL_Window* window;
  SDL_Renderer* renderer;

  std::unique_ptr<Registry> registry;
  std::unique_ptr<AssetStore> assetStore;
  std::unique_ptr<EventBus> eventBus;

  bool isRunning;
  bool isDebug;
  SDL_Rect camera;
  int millisecsPreviousFrame;
};
