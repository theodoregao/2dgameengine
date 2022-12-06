#pragma once

#include <SDL2/SDL.h>

#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Game/Game.h"

class CameraMovementSystem : public System {
 public:
  CameraMovementSystem() {
    RequireComponent<CameraFollowComponent>();
    RequireComponent<TransformComponent>();
  }

  void Update(SDL_Rect& camera) {
    for (auto entity : GetSystemEntities()) {
      auto transform = entity.GetComponent<TransformComponent>();
      if (transform.position.x + camera.w / 2 < Game::mapWidth) {
        camera.x = transform.position.x - Game::windowWidth / 2;
      }
      if (transform.position.y + camera.h / 2 < Game::mapHeight) {
        camera.y = transform.position.y - Game::windowHeight / 2;
      }
      camera.x = camera.x < 0 ? 0 : camera.x;
      camera.y = camera.y < 0 ? 0 : camera.y;
    }
  }
};
