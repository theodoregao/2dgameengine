#pragma once

#include <bitset>
#include <deque>
#include <memory>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "../Logger/Logger.h"

const size_t MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

class Entity {
 public:
  Entity(size_t id) : id(id) {}
  Entity(const Entity& entity) = default;
  size_t GetId() const;

  template <typename TComponent, typename... TArgs>
  void AddComponent(TArgs&&... args);
  template <typename TComponent>
  void RemoveComponent();
  template <typename TComponent>
  bool HasComponent() const;
  template <typename TComponent>
  TComponent& GetComponent() const;
  void Kill();

  Entity& operator=(const Entity& other) = default;
  bool operator==(const Entity& other) const { return id == other.id; }
  bool operator!=(const Entity& other) const { return id != other.id; }
  bool operator<(const Entity& other) const { return id < other.id; }
  bool operator>(const Entity& other) const { return id > other.id; }

  class Registry* registry;

 private:
  size_t id;
};

struct IComponent {
 protected:
  static size_t nextId;
};

template <typename T>
class Component : public IComponent {
 public:
  static size_t GetId() {
    static auto id = nextId++;
    return id;
  }
};

class System {
 public:
  System() = default;
  ~System() = default;
  void AddEntityToSystem(Entity entity);
  void RemoveEntityFromSystem(Entity entity);
  std::vector<Entity> GetSystemEntities() const;
  const Signature& GetComponentSignature() const;
  template <typename TComponent>
  void RequireComponent();

 private:
  Signature componentSignature;
  std::vector<Entity> entities;
};

class IPool {
 public:
  virtual ~IPool() {}
};

template <typename T>
class Pool : public IPool {
 public:
  Pool(size_t size = 100) { Resize(size); }
  virtual ~Pool() = default;
  bool IsEmpty() const { return data.empty(); }
  size_t GetSize() const { return data.size(); }
  void Resize(size_t n) { data.resize(n); }
  void Clear() { data.clear(); }
  void Add(T object) { data.push_back(object); }
  void Set(size_t index, T object) { data[index] = object; }
  T& Get(size_t index) { return static_cast<T&>(data[index]); }
  T& operator[](size_t index) { return data[index]; }

 private:
  std::vector<T> data;
};

class Registry {
 public:
  Registry() {
    Logger::Log(typeid(this).name(), "Registry contructor called");
  };
  ~Registry() {
    Logger::Log(typeid(this).name(), "Registry destructor called");
  };

  void Update();

  // Entity management
  Entity CreateEntity();
  void KillEntity(Entity entity);

  // Component management
  template <typename TComponent, typename... TArgs>
  void AddComponent(Entity entity, TArgs&&... args);
  template <typename TComponent>
  void RemoveComponent(Entity entity);
  template <typename TComponent>
  bool HasComponent(Entity entity) const;
  template <typename TComponent>
  TComponent& GetComponent(Entity entity) const;

  // System Management
  template <typename TSystem, typename... TArgs>
  void AddSystem(TArgs&&... args);
  template <typename TSystem>
  void RemoveSystem();
  template <typename TSystem>
  bool HasSystem() const;
  template <typename TSystem>
  TSystem& GetSystem() const;

 private:
  void AddEntityToSystem(Entity entity);
  void RemoveEntityFromSystem(Entity entity);

 private:
  size_t numEntities = 0;
  std::deque<size_t> freeIds;
  std::vector<std::shared_ptr<IPool>> componentPools;
  std::vector<Signature> entityComponentSignatures;
  std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
  std::set<Entity> entitiesToBeAdded;
  std::set<Entity> entitiesToBeKilled;
};

template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs&&... args) {
  registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
  registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const {
  return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
  return registry->GetComponent<TComponent>(*this);
}

template <typename TComponent>
void System::RequireComponent() {
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs&&... args) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  if (componentId >= componentPools.size()) {
    componentPools.resize(componentId + 1, nullptr);
  }

  if (!componentPools[componentId]) {
    std::shared_ptr<Pool<TComponent>> newComponentPool =
        std::make_shared<Pool<TComponent>>();
    componentPools[componentId] = newComponentPool;
  }

  std::shared_ptr<Pool<TComponent>> componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

  if (entityId >= componentPool->GetSize()) {
    componentPool->Resize(numEntities);
  }

  TComponent newComponent(std::forward<TArgs>(args)...);
  componentPool->Set(entityId, newComponent);

  entityComponentSignatures[entityId].set(componentId);

  Logger::Log(typeid(this).name(),
              "Component id " + std::to_string(componentId) +
                  " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();
  entityComponentSignatures[entityId].set(componentId, false);

  Logger::Log(typeid(this).name(),
              "Component id " + std::to_string(componentId) +
                  " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();
  return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();
  auto componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
  return componentPool->Get(entityId);
}

template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs&&... args) {
  std::shared_ptr<TSystem> newSystem =
      std::make_shared<TSystem>(std::forward<TArgs>(args)...);
  systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem() {
  auto system = systems.find(std::type_index(typeid(TSystem)));
  systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
  return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
  auto system = systems.find(std::type_index(typeid(TSystem)));
  return *(std::static_pointer_cast<TSystem>(system->second));
}
