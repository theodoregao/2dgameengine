#include "Game.h"

#include <fstream>
#include <glm/glm.hpp>
#include <typeinfo>

#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"

Game::Game() {
  Logger::Log(typeid(this).name(), "Game contructor called");
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();
  isRunning = true;
}

Game::~Game() { Logger::Log(typeid(this).name(), "Game destructor called"); }

void Game::Initialize() {
  Logger::Log(typeid(this).name(), "Initialize()");
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err(typeid(this).name(), "Error initializing SDL.");
    return;
  }
  windowWidth = GAME_WINDOW_WIDTH;
  windowHeight = GAME_WINDOW_HEIGHT;
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    Logger::Err(typeid(this).name(), "Error creating SDL window.");
    Logger::Err(typeid(this).name(), SDL_GetError());
    return;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    Logger::Err(typeid(this).name(), "Error creating SDL renderer.");
    Logger::Err(typeid(this).name(), SDL_GetError());
    return;
  }
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Game::Run() {
  Logger::Log(typeid(this).name(), "Run()");
  Setup();
  while (isRunning) {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::Setup() { LoadLevel(1); }

void Game::LoadLevel(int level) {
  Logger::Log(typeid(this).name(), "Setup()");
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();

  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper.png");
  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper.png");
  assetStore->AddTexture(renderer, "tank-image",
                         "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image",
                         "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
  assetStore->AddTexture(renderer, "tilemap-image",
                         "./assets/tilemaps/jungle.png");
  int tileSize = 32;
  double tileScale = 1.0;
  int mapNumCols = 25;
  int mapNumRows = 20;
  std::fstream mapFile;
  mapFile.open("./assets/tilemaps/jungle.map");
  for (int y = 0; y < mapNumRows; y++) {
    for (int x = 0; x < mapNumCols; x++) {
      char ch;
      mapFile.get(ch);
      int srcRectY = std::atoi(&ch) * tileSize;
      mapFile.get(ch);
      int srcRectX = std::atoi(&ch) * tileSize;
      mapFile.ignore();

      Entity tile = registry->CreateEntity();
      tile.AddComponent<TransformComponent>(
          glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
          glm::vec2(tileScale, tileScale), 0.0);
      tile.AddComponent<SpriteComponent>("tilemap-image", 0, tileSize, tileSize,
                                         srcRectX, srcRectY);
    }
  }

  Entity chopper = registry->CreateEntity();
  chopper.AddComponent<TransformComponent>(glm::vec2(200, 200),
                                           glm::vec2(1.0, 1.0), 0.0);
  chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  chopper.AddComponent<SpriteComponent>("chopper-image", 3, 32, 32);
  chopper.AddComponent<AnimationComponent>(2, 15);

  Entity tank = registry->CreateEntity();
  tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 50.0));
  tank.AddComponent<SpriteComponent>("tank-image", 2, 32, 32);
  tank.AddComponent<BoxColliderComponent>(32, 32);

  Entity truck = registry->CreateEntity();
  truck.AddComponent<TransformComponent>(glm::vec2(500.0, 500.0),
                                         glm::vec2(2.0, 2.0), -135.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, -30.0));
  truck.AddComponent<SpriteComponent>("truck-image", 1, 32, 32);
  truck.AddComponent<BoxColliderComponent>(32, 32);

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 70, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  radar.AddComponent<SpriteComponent>("radar-image", 1, 64, 64);
  radar.AddComponent<AnimationComponent>(8, 5);
}

void Game::ProcessInput() {
  SDL_Event sdlEvent;
  while (isRunning && SDL_PollEvent(&sdlEvent)) {
    switch (sdlEvent.type) {
      case SDL_QUIT:
        isRunning = false;
        break;

      case SDL_KEYDOWN:
        if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
          isRunning = false;
        }
        break;

      default:
        break;
    }
  }
}

void Game::Update() {
  // FPS control
  int timeToWait =
      MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
  if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
    SDL_Delay(timeToWait);
  }

  // use delta time to control speed
  double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
  millisecsPreviousFrame = SDL_GetTicks();

  // Update the registry to process the entities that are waiting to be
  // created/deleted
  registry->Update();

  // Ask all the systems to update
  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CollisionSystem>().Update();
}

void Game::Render() {
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);
  registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
  SDL_RenderPresent(renderer);  // swap the back & front buffer
}

void Game::Destory() {
  Logger::Log(typeid(this).name(), "Destory()");
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
