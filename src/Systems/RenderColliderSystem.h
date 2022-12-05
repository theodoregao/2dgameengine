#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <iterator>
#include <set>

#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"

class RenderColliderSystem : public System {
 public:
  RenderColliderSystem() {
    RequireComponent<BoxColliderComponent>();
    RequireComponent<TransformComponent>();
  }

  void Update(SDL_Renderer* renderer) {
    auto entities = GetSystemEntities();
    std::set<Entity> collidedEntities;
    for (auto i = entities.begin(); i != entities.end(); i++) {
      Entity a = *i;
      auto aTransform = a.GetComponent<TransformComponent>();
      auto aCollider = a.GetComponent<BoxColliderComponent>();
      for (auto j = i + 1; j != entities.end(); j++) {
        Entity b = *j;
        if (a == b) continue;
        auto bTransform = b.GetComponent<TransformComponent>();
        auto bCollider = b.GetComponent<BoxColliderComponent>();
        auto isCollides = CollisionUtil::CheckAABBCollision(
            aTransform.position + aCollider.offset,
            glm::vec2(aCollider.width, aCollider.height),
            bTransform.position + bCollider.offset,
            glm::vec2(bCollider.width, bCollider.height));
        if (isCollides) {
          collidedEntities.insert(a);
          collidedEntities.insert(b);
        }
      }

      SDL_Rect colliderRect = {
          static_cast<int>(aTransform.position.x + aCollider.offset.x),
          static_cast<int>(aTransform.position.y + aCollider.offset.y),
          static_cast<int>(aCollider.width),
          static_cast<int>(aCollider.height)};
      glm::vec4 color(0, 0, 0, 255);
      if (collidedEntities.find(a) != collidedEntities.end()) color.r = 255;
      SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.w);
      SDL_RenderDrawRect(renderer, &colliderRect);
    }
  }
};
