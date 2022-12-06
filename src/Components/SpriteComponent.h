#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <string>

struct SpriteComponent {
  std::string assetId;
  size_t zIndex;
  int width;
  int height;
  bool isFixed;
  SDL_Rect srcRect;

  SpriteComponent(std::string assetId = "", size_t zIndex = 0, int width = 0,
                  int height = 0, bool isFixed = false, int srcRectX = 0,
                  int srcRectY = 0)
      : assetId(assetId),
        zIndex(zIndex),
        width(width),
        height(height),
        isFixed(isFixed) {
    srcRect = {srcRectX, srcRectY, width, height};
  }
};
