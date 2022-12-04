#pragma once

#include <SDL2/SDL.h>

#include <map>
#include <string>

class AssetStore {
 public:
  AssetStore();
  ~AssetStore();
  void ClearAssets();
  void AddTexture(SDL_Renderer* renderer, const std::string& assetId,
                  const std::string& filePath);
  SDL_Texture* GetTexture(const std::string& assetId);

 private:
  std::map<std::string, SDL_Texture*> textures;
};
