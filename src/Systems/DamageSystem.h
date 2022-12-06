#pragma once

#include <memory>

#include "../Components/BoxColliderComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Logger/Logger.h"

class DamageSystem : public System {
 public:
  DamageSystem() { RequireComponent<BoxColliderComponent>(); }

  void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
    eventBus->SubscribeToEvent<CollisionEvent>(this,
                                               &DamageSystem::OnCollision);
  }

  void OnCollision(CollisionEvent& event) {
    Logger::Log(LOG_CLASS_TAG, std::to_string(event.a.GetId()) +
                                   " collides with " +
                                   std::to_string(event.b.GetId()));
    event.a.Kill();
    event.b.Kill();
  }

  void Update() {}
};
