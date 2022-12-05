#pragma once

#include <SDL2/SDL.h>

#include "../Components/AnimationComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"

class AnimationSystem : public System {
 public:
  AnimationSystem() {
    RequireComponent<SpriteComponent>();
    RequireComponent<AnimationComponent>();
  }

  void Update() {
    for (auto entity : GetSystemEntities()) {
      auto& animation = entity.GetComponent<AnimationComponent>();
      auto& sprite = entity.GetComponent<SpriteComponent>();

      animation.currentFrame = (SDL_GetTicks() - animation.startTime) *
                               animation.frameRateSpeed / 1000 %
                               animation.numFrames;
      sprite.srcRect.x = animation.currentFrame * sprite.width;
    }
  }
};
