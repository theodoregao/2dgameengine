#pragma once

#include <SDL2/SDL.h>

#include "../Components/ProjectileComponent.h"
#include "../ECS/ECS.h"

class ProjectileLifecycleSystem : public System {
 public:
  ProjectileLifecycleSystem() { RequireComponent<ProjectileComponent>(); }

  void Update() {
    for (auto entity : GetSystemEntities()) {
      auto projectile = entity.GetComponent<ProjectileComponent>();
      if (static_cast<int>(SDL_GetTicks()) - projectile.startTime >
          projectile.duration) {
        entity.Kill();
      }
    }
  }
};
