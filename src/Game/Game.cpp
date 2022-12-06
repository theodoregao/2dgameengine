#include "Game.h"

#include <SDL2/SDL.h>

#include <fstream>
#include <glm/glm.hpp>

#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Events/KeyPressedEvent.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/RenderSystem.h"

int Game::windowWidth = 800;
int Game::windowHeight = 600;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
  Logger::Log(LOG_CLASS_TAG, "Game contructor called");
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();
  eventBus = std::make_unique<EventBus>();
  isRunning = true;
  isDebug = false;
}

Game::~Game() { Logger::Log(LOG_CLASS_TAG, "Game destructor called"); }

void Game::Initialize() {
  Logger::Log(LOG_CLASS_TAG, "Initialize()");
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err(LOG_CLASS_TAG, "Error initializing SDL.");
    return;
  }
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    Logger::Err(LOG_CLASS_TAG, "Error creating SDL window.");
    Logger::Err(LOG_CLASS_TAG, SDL_GetError());
    return;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    Logger::Err(LOG_CLASS_TAG, "Error creating SDL renderer.");
    Logger::Err(LOG_CLASS_TAG, SDL_GetError());
    return;
  }
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  camera.x = 0;
  camera.y = 0;
  camera.w = windowWidth;
  camera.h = windowHeight;
}

void Game::Run() {
  Logger::Log(LOG_CLASS_TAG, "Run()");
  Setup();
  while (isRunning) {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::Setup() { LoadLevel(1); }

void Game::LoadLevel(int level) {
  Logger::Log(LOG_CLASS_TAG, "Setup()");
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<CameraMovementSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<DamageSystem>();
  registry->AddSystem<KeyboardControlSystem>();
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<ProjectileEmitSystem>();
  registry->AddSystem<ProjectileLifecycleSystem>();
  registry->AddSystem<RenderColliderSystem>();
  registry->AddSystem<RenderSystem>();

  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper-spritesheet.png");
  assetStore->AddTexture(renderer, "tank-image",
                         "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image",
                         "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
  assetStore->AddTexture(renderer, "tilemap-image",
                         "./assets/tilemaps/jungle.png");
  assetStore->AddTexture(renderer, "bullet-image",
                         "./assets/images/bullet.png");
  int tileSize = 32;
  double tileScale = 2.0;
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
                                         false, srcRectX, srcRectY);
    }
  }
  mapWidth = mapNumCols * tileSize * tileScale;
  mapHeight = mapNumRows * tileSize * tileScale;

  Entity chopper = registry->CreateEntity();
  chopper.AddComponent<TransformComponent>(
      glm::vec2(windowWidth / 2, windowHeight / 2), glm::vec2(1.0, 1.0), 0.0);
  chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  chopper.AddComponent<SpriteComponent>("chopper-image", 2, 32, 32);
  chopper.AddComponent<AnimationComponent>(2, 15);
  int speed = 80;
  chopper.AddComponent<KeyboardControlledComponent>(
      glm::vec2(0, -speed), glm::vec2(speed, 0), glm::vec2(0, speed),
      glm::vec2(-speed, 0));
  chopper.AddComponent<CameraFollowComponent>();
  chopper.AddComponent<HealthComponent>(100);
  chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 500,
                                                   10000, 0, true);

  Entity tank = registry->CreateEntity();
  tank.AddComponent<TransformComponent>(glm::vec2(110.0, 500.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 1, 32, 32);
  tank.AddComponent<BoxColliderComponent>(32, 32);
  tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(50.0, 50.0), 2000,
                                                5000, 0);
  tank.AddComponent<HealthComponent>(100);

  Entity truck = registry->CreateEntity();
  truck.AddComponent<TransformComponent>(glm::vec2(500.0, 450.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  truck.AddComponent<SpriteComponent>("truck-image", 1, 32, 32);
  truck.AddComponent<BoxColliderComponent>(32, 32);
  truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(-50.0, 50.0), 3000,
                                                 5000, 0);
  truck.AddComponent<HealthComponent>(100);

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 70, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  radar.AddComponent<SpriteComponent>("radar-image", 1, 64, 64, true);
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
        } else if (sdlEvent.key.keysym.sym == SDLK_d) {
          isDebug = !isDebug;
        }
        eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
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

  // Perform the subscription of the events for all systems
  eventBus->Reset();
  registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
  registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
  registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

  // Update the registry to process the entities that are waiting to be
  // created/deleted
  registry->Update();

  // Ask all the systems to update
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CameraMovementSystem>().Update(camera);
  registry->GetSystem<CollisionSystem>().Update(eventBus);
  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<ProjectileEmitSystem>().Update(registry);
  registry->GetSystem<ProjectileLifecycleSystem>().Update();
}

void Game::Render() {
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);
  registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
  if (isDebug) {
    registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
  }
  SDL_RenderPresent(renderer);  // swap the back & front buffer
}

void Game::Destory() {
  Logger::Log(LOG_CLASS_TAG, "Destory()");
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
