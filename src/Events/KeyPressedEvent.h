#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class KeyPressedEvent : public Event {
 public:
  int symbol;
  KeyPressedEvent(int symbol) : symbol(symbol) {}
};
