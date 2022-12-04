#include "ECS.h"

#include <algorithm>

#include "../Logger/Logger.h"

size_t IComponent::nextId = 0;

size_t Entity::GetId() const { return id; }

void System::AddEntityToSystem(Entity entity) { entities.push_back(entity); }

void System::RemoveEntityFromSystem(Entity entity) {
  entities.erase(
      std::remove_if(entities.begin(), entities.end(),
                     [&entity](Entity other) { return entity == other; }),
      entities.end());
}

std::vector<Entity> System::GetSystemEntities() const { return entities; }

const Signature& System::GetComponentSignature() const {
  return componentSignature;
}

Entity Registry::CreateEntity() {
  int entityId = numEntities++;
  if (static_cast<size_t>(entityId) >= entityComponentSignatures.size()) {
    entityComponentSignatures.resize(entityId + 1);
  }
  Entity entity(entityId);
  entity.registry = this;
  entitiesToBeAdded.insert(entity);
  Logger::Log(typeid(this).name(),
              "Entity creawted with id = " + std::to_string(entityId));
  return entity;
}

void Registry::Update() {
  for (auto entity : entitiesToBeAdded) {
    AddEntityToSystems(entity);
  }
  entitiesToBeAdded.clear();
}

void Registry::AddEntityToSystems(Entity entity) {
  const auto entityId = entity.GetId();
  const auto entityComponentSignature = entityComponentSignatures[entityId];
  for (auto& system : systems) {
    const auto& systemComponentSignature =
        system.second->GetComponentSignature();
    bool isInterested = (entityComponentSignature & systemComponentSignature) ==
                        systemComponentSignature;
    if (isInterested) {
      system.second->AddEntityToSystem(entity);
    }
  }
}
