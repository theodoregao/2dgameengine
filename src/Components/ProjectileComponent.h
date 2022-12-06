#pragma once

#include <SDL2/SDL.h>

struct ProjectileComponent {
  int hitPercentDamage;
  int duration;
  bool isFriendly;
  int startTime;

  ProjectileComponent(int hitPercentDamage = 0, int duration = 0,
                      bool isFriendly = false)
      : hitPercentDamage(hitPercentDamage),
        duration(duration),
        isFriendly(isFriendly) {
    startTime = SDL_GetTicks();
  }
};
