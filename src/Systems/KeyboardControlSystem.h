#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Logger/Logger.h"

class KeyboardControlSystem : public System {
 public:
  KeyboardControlSystem() {}

  void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
    eventBus->SubscribeToEvent<KeyPressedEvent>(
        this, &KeyboardControlSystem::OnKeyPressed);
  }

  void OnKeyPressed(KeyPressedEvent& event) {
    std::string keyCode = std::to_string(event.symbol);
    std::string keySymbol(1, event.symbol);
    Logger::Log(typeid(this).name(),
                "Key pressed event emitted: [" + keyCode + "] " + keySymbol);
  }

  void Update() {}
};
