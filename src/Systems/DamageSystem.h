#pragma once

#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
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
    Entity a = event.a;
    Entity b = event.b;

    if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
      OnProjectileHitsPlayer(a, b);
    }

    if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
      OnProjectileHitsPlayer(b, a);
    }

    if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
      OnProjecttileHitsEnermy(a, b);
    }

    if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
      OnProjecttileHitsEnermy(b, a);
    }
  }

  void Update() {}

  void OnProjectileHitsPlayer(Entity projectile, Entity player) {
    auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
    if (!projectileComponent.isFriendly) {
      auto& health = player.GetComponent<HealthComponent>();
      health.healthPercentage -= projectileComponent.hitPercentDamage;

      if (health.healthPercentage <= 0) {
        player.Kill();
      }
      projectile.Kill();
    }
  }

  void OnProjecttileHitsEnermy(Entity projectile, Entity enermy) {
    auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
    if (projectileComponent.isFriendly) {
      auto& health = enermy.GetComponent<HealthComponent>();
      health.healthPercentage -= projectileComponent.hitPercentDamage;

      if (health.healthPercentage <= 0) {
        enermy.Kill();
      }
      projectile.Kill();
    }
  }
};
