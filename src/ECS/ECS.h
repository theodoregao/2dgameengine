#pragma once

#include <bitset>
#include <deque>
#include <memory>
#include <set>
#include <typeindex>
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

  // Tag & Group management
  void Tag(const std::string tag);
  bool HasTag(const std::string tag) const;
  void Group(const std::string group);
  bool BelongsToGroup(const std::string group) const;

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
  virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool : public IPool {
 public:
  Pool(size_t capacity = 100) {
    size = 0;
    data.resize(capacity);
  }

  virtual ~Pool() = default;

  bool IsEmpty() const { return size == 0; }

  size_t GetSize() const { return size; }

  void Clear() {
    size = 0;
    data.clear();
  }

  void Set(size_t entityId, T object) {
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
      int index = entityIdToIndex[entityId];
      data[index] = object;
    } else {
      int index = size;
      entityIdToIndex.emplace(entityId, index);
      indexToEntityId.emplace(index, entityId);
      if (static_cast<long unsigned int>(index) >= data.capacity()) {
        data.resize(data.capacity() * 2);
      }
      data[index] = object;
      size++;
    }
  }

  void Remove(int entityId) {
    int indexOfRemoved = entityIdToIndex[entityId];
    int indexOfLast = size - 1;
    data[indexOfRemoved] = data[indexOfLast];

    int entityIdOfLastElement = indexToEntityId[indexOfLast];
    entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
    indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

    entityIdToIndex.erase(entityId);
    indexToEntityId.erase(indexOfLast);

    size--;
  }

  void RemoveEntityFromPool(int entityId) override {
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
      Remove(entityId);
    }
  }

  T& Get(size_t entityId) {
    return static_cast<T&>(data[entityIdToIndex[entityId]]);
  }

  T& operator[](size_t index) { return data[index]; }

 private:
  void Add(T object) { data.push_back(object); }
  void Resize(int n) { data.resize(n); }

 private:
  std::vector<T> data;
  int size;

  std::unordered_map<int, int> entityIdToIndex;
  std::unordered_map<int, int> indexToEntityId;
};

class Registry {
 public:
  Registry() { Logger::Log(LOG_CLASS_TAG, "Registry contructor called"); };
  ~Registry() { Logger::Log(LOG_CLASS_TAG, "Registry destructor called"); };

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

  // Tag management
  void TagEntity(Entity entity, const std::string& tag);
  bool EntityHasTag(Entity entity, const std::string& tag) const;
  Entity GetEntityByTag(const std::string& tag) const;
  void RemoveEntityTag(Entity entity);

  // Group management
  void GroupEntity(Entity entity, const std::string& group);
  bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
  std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
  void RemoveEntityGroup(Entity entity);

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

  std::unordered_map<std::string, Entity> entityPerTag;
  std::unordered_map<int, std::string> tagPerEntity;

  std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
  std::unordered_map<int, std::string> groupPerEntity;
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

  TComponent newComponent(std::forward<TArgs>(args)...);
  componentPool->Set(entityId, newComponent);

  entityComponentSignatures[entityId].set(componentId);

  Logger::Log(LOG_CLASS_TAG, "Component id " + std::to_string(componentId) +
                                 " was added to entity id " +
                                 std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  std::shared_ptr<Pool<TComponent>> componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
  componentPool[entityId]->Remove(entityId);

  entityComponentSignatures[entityId].set(componentId, false);

  Logger::Log(LOG_CLASS_TAG, "Component id " + std::to_string(componentId) +
                                 " was removed from entity id " +
                                 std::to_string(entityId));
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
