#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <string>

struct SpriteComponent {
  std::string assetId;
  size_t zIndex;
  int width;
  int height;
  SDL_Rect srcRect;

  SpriteComponent(std::string assetId = "", size_t zIndex = 0, int width = 0,
                  int height = 0, int srcRectX = 0, int srcRectY = 0)
      : assetId(assetId), zIndex(zIndex), width(width), height(height) {
    srcRect = {srcRectX, srcRectY, width, height};
  }
};
