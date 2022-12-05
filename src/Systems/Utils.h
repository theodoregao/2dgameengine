#pragma once

#include <glm/glm.hpp>

class CollisionUtil {
 public:
  static bool CheckAABBCollision(glm::vec2 aPosition, glm::vec2 aSize,
                                 glm::vec2 bPosition, glm::vec2 bSize) {
    return (aPosition.x < bPosition.x + bSize.x &&
            aPosition.x + aSize.x > bPosition.x &&
            aPosition.y < bPosition.y + bSize.y &&
            aPosition.y + aSize.y > bPosition.y);
  }
};
