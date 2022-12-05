#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <typeinfo>

#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Logger/Logger.h"
#include "Utils.h"

class CollisionSystem : public System {
 public:
  CollisionSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<BoxColliderComponent>();
  }

  void Update(std::unique_ptr<EventBus>& eventBus) {
    auto entities = GetSystemEntities();
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
          eventBus->EmitEvent<CollisionEvent>(a, b);
        }
      }
    }
  }
};
