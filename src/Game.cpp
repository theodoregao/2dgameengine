#include "Game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <glm/glm.hpp>
#include <iostream>

const int Game::GAME_WINDOW_WIDTH = 800;
const int Game::GAME_WINDOW_HEIGHT = 600;

Game::Game() {
  std::cout << "Game contructor called" << std::endl;
  isRunning = true;
}

Game::~Game() { std::cout << "Game destructor called" << std::endl; }

void Game::Initialize() {
  std::cout << "Initialize()" << std::endl;
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "Error initializing SDL." << std::endl;
    return;
  }
  windowWidth = Game::GAME_WINDOW_WIDTH;
  windowHeight = Game::GAME_WINDOW_HEIGHT;
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    std::cerr << "Error creating SDL window." << std::endl;
    std::cerr << SDL_GetError() << std::endl;
    return;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    std::cerr << "Error creating SDL renderer." << std::endl;
    std::cerr << SDL_GetError() << std::endl;
    return;
  }
  // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Game::Run() {
  std::cout << "Run()" << std::endl;
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
  std::cout << "Setup()" << std::endl;
  playerPosition = glm::vec2(10.0, 20.0);
  playerVelocity = glm::vec2(60.0, 0);
}

void Game::ProcessInput() {
  std::cout << "ProcessInput()" << std::endl;
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
  std::cout << "Update()" << std::endl;

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
  std::cout << "Render()" << std::endl;
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
  std::cout << "Destory()" << std::endl;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
