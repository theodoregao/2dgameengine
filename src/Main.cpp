#include <iostream>

#include "Game/Game.h"

int main() {
  Game game;
  game.Initialize();
  game.Run();
  game.Destory();
  return 0;
}
