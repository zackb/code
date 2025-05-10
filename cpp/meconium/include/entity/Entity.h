#pragma once

#include "Entity.h"
#include "components/Component.h"
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Entity {
public:
    int id;

    Entity(int id_) : id(id_) {}

    template <typename T> void addComponent(std::shared_ptr<T> component) {
        components[std::type_index(typeid(T))] = std::static_pointer_cast<void>(component);
    }

    // Get a component of type T
    template <typename T> std::shared_ptr<T> getComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    // Get all components for this entity (for indexing)
    const std::unordered_map<std::type_index, std::shared_ptr<void>>& getComponents() const { return components; }

    template <typename T> bool hasComponent() const { return components.contains(std::type_index(typeid(T))); }

private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> components;
};

// The Entities class that manages all entities and their components
class Entities {
public:
    // Add an entity to the system
    void add(const std::shared_ptr<Entity>& entity) {
        entities.push_back(entity);
        updateEntityIndex(entity);
    }

    // Get all entities
    std::vector<std::shared_ptr<Entity>>& all() { return entities; }

    // Iterator support
    auto begin() { return entities.begin(); }

    auto end() { return entities.end(); }

    auto begin() const { return entities.begin(); }

    auto end() const { return entities.end(); }

    // Find the first entity with a specific component
    template <typename T> std::shared_ptr<T> findFirstComponent() {
        auto& index = componentIndex<T>();
        if (!index.empty()) {
            return std::static_pointer_cast<T>(index.begin()->second);
        }
        return nullptr;
    }

    // Find all entities with a specific component
    template <typename T> std::vector<std::shared_ptr<T>> findAllComponents() {
        std::vector<std::shared_ptr<T>> result;
        auto& index = componentIndex<T>();
        for (auto& entry : index) {
            result.push_back(std::static_pointer_cast<T>(entry.second));
        }
        return result;
    }

    template <typename Component>
    std::shared_ptr<Entity> findEntityWithComponent() {
        for (auto& entity : *this) {
            if (entity->hasComponent<Component>()) {
                return entity;
            }
        }
        return nullptr;
    }

    // Find entities by a combination of components (e.g., Transform + Sprite)
    template <typename... Components> std::vector<std::shared_ptr<Entity>> findByComponents() const {
        std::vector<std::shared_ptr<Entity>> result;
        for (auto& e : entities) {
            if (hasComponents<Components...>(e)) {
                result.push_back(e);
            }
        }
        return result;
    }

    // Apply a filter function to find entities based on custom conditions
    template <typename FilterFn> std::vector<std::shared_ptr<Entity>> findByFilter(FilterFn&& filter) {
        std::vector<std::shared_ptr<Entity>> result;
        for (auto& e : entities) {
            if (filter(e)) {
                result.push_back(e);
            }
        }
        return result;
    }

private:
    // Update the component index for a specific entity
    void updateEntityIndex(const std::shared_ptr<Entity>& entity) {
        for (auto& comp : entity->getComponents()) {
            componentIndexInternal(comp.first).insert({entity->id, comp.second});
        }
    }

    // Helper function to get the component index for a specific type
    template <typename T> std::unordered_map<int, std::shared_ptr<void>>& componentIndex() {
        return componentIndexInternal(typeid(T));
    }

    // Internal storage of components by type
    std::unordered_map<std::type_index, std::unordered_map<int, std::shared_ptr<void>>> componentStorage;

    // Returns the internal index for a specific component type
    std::unordered_map<int, std::shared_ptr<void>>& componentIndexInternal(std::type_index type) {
        return componentStorage[type];
    }

    // Helper function to check if an entity has all required components
    template <typename... Components> bool hasComponents(const std::shared_ptr<Entity>& e) const {
        return (... && (e->getComponent<Components>() != nullptr));
    }

    // Store all entities in a list
    std::vector<std::shared_ptr<Entity>> entities;
};
