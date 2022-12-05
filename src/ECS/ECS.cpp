#include "ECS.h"

#include <algorithm>

#include "../Logger/Logger.h"

size_t IComponent::nextId = 0;

size_t Entity::GetId() const { return id; }

void Entity::Kill() { registry->KillEntity(*this); }

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
  size_t entityId = 0;
  if (freeIds.empty()) {
    entityId = numEntities++;
    if (entityId >= entityComponentSignatures.size()) {
      entityComponentSignatures.resize(entityId + 1);
    }
  } else {
    entityId = freeIds.front();
    freeIds.pop_front();
  }

  Entity entity(entityId);
  entity.registry = this;
  entitiesToBeAdded.insert(entity);
  Logger::Log(typeid(this).name(),
              "Entity creawted with id = " + std::to_string(entityId));
  return entity;
}

void Registry::KillEntity(Entity entity) {
  entitiesToBeKilled.insert(entity);
  Logger::Log(typeid(this).name(),
              "Entity killed with id = " + std::to_string(entity.GetId()));
}

void Registry::Update() {
  for (auto entity : entitiesToBeAdded) {
    AddEntityToSystem(entity);
  }
  entitiesToBeAdded.clear();

  for (auto entity : entitiesToBeKilled) {
    RemoveEntityFromSystem(entity);
    entityComponentSignatures[entity.GetId()].reset();
    freeIds.push_back(entity.GetId());
  }
  entitiesToBeKilled.clear();
}

void Registry::AddEntityToSystem(Entity entity) {
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

void Registry::RemoveEntityFromSystem(Entity entity) {
  for (auto& system : systems) {
    system.second->RemoveEntityFromSystem(entity);
  }
}
