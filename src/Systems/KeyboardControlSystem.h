#pragma once

#include <SDL2/SDL.h>

#include "../Components/KeyboardControlledComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardControlSystem : public System {
 public:
  KeyboardControlSystem() {
    RequireComponent<KeyboardControlledComponent>();
    RequireComponent<RigidBodyComponent>();
    RequireComponent<SpriteComponent>();
  }

  void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
    eventBus->SubscribeToEvent<KeyPressedEvent>(
        this, &KeyboardControlSystem::OnKeyPressed);
  }

  void OnKeyPressed(KeyPressedEvent& event) {
    for (auto entity : GetSystemEntities()) {
      const auto keyboardControl =
          entity.GetComponent<KeyboardControlledComponent>();
      auto& sprite = entity.GetComponent<SpriteComponent>();
      auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

      switch (event.symbol) {
        case SDLK_UP:
          rigidbody.velocity = keyboardControl.upVelocity;
          sprite.srcRect.y = sprite.height * 0;
          break;
        case SDLK_RIGHT:
          rigidbody.velocity = keyboardControl.rightVelocity;
          sprite.srcRect.y = sprite.height * 1;
          break;
        case SDLK_DOWN:
          rigidbody.velocity = keyboardControl.downVelocity;
          sprite.srcRect.y = sprite.height * 2;
          break;
        case SDLK_LEFT:
          rigidbody.velocity = keyboardControl.leftVelocity;
          sprite.srcRect.y = sprite.height * 3;
          break;
        default:
          break;
      }
    }
  }

  void Update() {}
};
