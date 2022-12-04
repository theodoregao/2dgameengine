#include "AssetStore.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <typeinfo>

#include "../Logger/Logger.h"

AssetStore::AssetStore() {
  Logger::Log(typeid(this).name(), "AssetStore constructor called!");
}

AssetStore::~AssetStore() {
  ClearAssets();
  Logger::Log(typeid(this).name(), "AssetStore destructor called!");
}

void AssetStore::ClearAssets() {
  for (auto texture : textures) {
    SDL_DestroyTexture(texture.second);
  }
  textures.clear();
}

void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId,
                            const std::string& filePath) {
  SDL_Surface* surface = IMG_Load(filePath.c_str());
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  textures.emplace(assetId, texture);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
  return textures[assetId];
}
