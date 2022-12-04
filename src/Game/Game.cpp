#include "Game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <glm/glm.hpp>

#include "../Logger/Logger.h"

const int Game::GAME_WINDOW_WIDTH = 800;
const int Game::GAME_WINDOW_HEIGHT = 600;

Game::Game() {
  Logger::Log("Game contructor called");
  isRunning = true;
}

Game::~Game() { Logger::Log("Game destructor called"); }

void Game::Initialize() {
  Logger::Log("Initialize()");
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err("Error initializing SDL.");
    return;
  }
  windowWidth = Game::GAME_WINDOW_WIDTH;
  windowHeight = Game::GAME_WINDOW_HEIGHT;
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    Logger::Err("Error creating SDL window.");
    Logger::Err(SDL_GetError());
    return;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    Logger::Err("Error creating SDL renderer.");
    Logger::Err(SDL_GetError());
    return;
  }
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Game::Run() {
  Logger::Log("Run()");
  Setup();
  while (isRunning) {
    ProcessInput();
    Update();
    Render();
  }
}

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup() {
  Logger::Log("Setup()");
  playerPosition = glm::vec2(10.0, 20.0);
  playerVelocity = glm::vec2(60.0, 0);
}

void Game::ProcessInput() {
  Logger::Log("ProcessInput()");
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
  Logger::Log("Update()");

  // FPS control
  int timeToWait =
      MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
  if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
    SDL_Delay(timeToWait);
  }

  // use delta time to control speed
  double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
  millisecsPreviousFrame = SDL_GetTicks();

  playerPosition.x += playerVelocity.x * deltaTime;
  playerPosition.y += playerVelocity.y * deltaTime;
}

void Game::Render() {
  Logger::Log("Render()");
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);

  SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  SDL_Rect dstRect = {static_cast<int>(playerPosition.x),
                      static_cast<int>(playerPosition.y), 32, 32};
  SDL_RenderCopy(renderer, texture, NULL, &dstRect);
  SDL_DestroyTexture(texture);

  SDL_RenderPresent(renderer);  // swap the back & front buffer
}

void Game::Destory() {
  Logger::Log("Destory()");
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
