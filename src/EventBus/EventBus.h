#pragma once

#include <functional>
#include <list>
#include <map>
#include <typeindex>

#include "../Logger/Logger.h"
#include "Event.h"

class IEventCallback {
 public:
  virtual ~IEventCallback() = default;
  void Execute(Event& e) { Call(e); }

 private:
  virtual void Call(Event& e) = 0;
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
  typedef void (TOwner::*CallbackFunction)(TEvent&);

 public:
  EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
    this->ownerInstance = ownerInstance;
    this->callbackFunction = callbackFunction;
  }

 private:
  virtual void Call(Event& e) override {
    std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
  }

 private:
  TOwner* ownerInstance;
  CallbackFunction callbackFunction;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
 public:
  EventBus() { Logger::Log(LOG_CLASS_TAG, "contructor called"); };
  ~EventBus() { Logger::Log(LOG_CLASS_TAG, "destructor called"); };

  void Reset() { subscribers.clear(); }

  template <typename TEvent, typename TOwner>
  void SubscribeToEvent(TOwner* ownerInstance,
                        void (TOwner::*callbackFunction)(TEvent&)) {
    if (!subscribers[typeid(TEvent)].get()) {
      subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
    }
    auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(
        ownerInstance, callbackFunction);
    subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
  }

  template <typename TEvent, typename... TArgs>
  void EmitEvent(TArgs&&... args) {
    auto handlers = subscribers[typeid(TEvent)].get();
    if (handlers) {
      for (auto it = handlers->begin(); it != handlers->end(); it++) {
        auto handler = it->get();
        TEvent event(std::forward<TArgs>(args)...);
        handler->Execute(event);
      }
    }
  }

 private:
  std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
};
