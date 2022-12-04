#include "Game.h"

#include <SDL2/SDL.h>

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
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Game::Run() {
  std::cout << "Run()" << std::endl;
  while (isRunning) {
    ProcessInput();
    Update();
    Render();
  }
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

void Game::Update() { std::cout << "Update()" << std::endl; }

void Game::Render() {
  std::cout << "Render()" << std::endl;
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

void Game::Destory() {
  std::cout << "Destory()" << std::endl;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
