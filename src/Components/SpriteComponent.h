#pragma once

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <string>

struct SpriteComponent {
  std::string assetId;
  int width;
  int height;
  SDL_Rect srcRect;

  SpriteComponent(std::string assetId = "", int width = 0, int height = 0,
                  int srcRectX = 0, int srcRectY = 0)
      : assetId(assetId), width(width), height(height) {
    srcRect = {srcRectX, srcRectY, width, height};
  }
};
