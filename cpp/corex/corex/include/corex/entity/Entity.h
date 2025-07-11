#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Entity {
public:
    int id;

    Entity() {
        static int ids = 9999;
        id = ids++;
    }
    explicit Entity(int id_) : id(id_) {}

    template <typename T, typename... Args> std::shared_ptr<T> addComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        components[std::type_index(typeid(T))] = std::static_pointer_cast<void>(component);
        return component;
    }

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
    template <typename T> void removeComponent() { components.erase(std::type_index(typeid(T))); }

    std::unordered_map<std::type_index, std::shared_ptr<void>> components;

    // only let entities remove components
    friend class Entities;
};

// helper class to iterate over the collection without making vectors like in findBy*
template <typename... Components> class FilteredView {
    using EntityPtr = std::shared_ptr<Entity>;

    const std::vector<EntityPtr>& entities;
    size_t index = 0;

    // Advance index to the next entity that has all Components
    void skipToNextValid() {
        while (index < entities.size() && !hasComponents(entities[index])) {
            ++index;
        }
    }

public:
    FilteredView(const std::vector<EntityPtr>& ents) : entities(ents), index(0) { skipToNextValid(); }

    class iterator {
        const std::vector<EntityPtr>& entities;
        size_t pos;

        void skipToNextValid() {
            while (pos < entities.size() && !hasComponents(entities[pos])) {
                ++pos;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = EntityPtr;
        using difference_type = std::ptrdiff_t;
        using pointer = const EntityPtr*;
        using reference = const EntityPtr&;

        iterator(const std::vector<EntityPtr>& ents, size_t startPos) : entities(ents), pos(startPos) {
            skipToNextValid();
        }

        reference operator*() const { return entities[pos]; }
        pointer operator->() const { return &entities[pos]; }

        iterator& operator++() {
            ++pos;
            skipToNextValid();
            return *this;
        }

        bool operator==(const iterator& other) const { return pos == other.pos; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    iterator begin() const { return iterator(entities, index); }
    iterator end() const { return iterator(entities, entities.size()); }

private:
    static bool hasComponents(const EntityPtr& e) { return (... && (e->getComponent<Components>() != nullptr)); }
};

// The Entities class that manages all entities and their components
class Entities {
public:
    // Add an entity to the system
    void add(const std::shared_ptr<Entity>& entity) {
        if (!entity) {
            std::cerr << "Warning: adding null entity!" << std::endl;
            return;
        }
        entities.push_back(entity);
        updateEntityIndex(entity);
    }

    // Remove an entity from the system
    void remove(const std::shared_ptr<Entity>& entity) {
        entities.erase(std::remove_if(entities.begin(),
                                      entities.end(),
                                      [&entity](const std::shared_ptr<Entity>& e) { return e->id == entity->id; }),
                       entities.end());

        // Remove this entity's components from all component indexes
        for (auto& [type, index] : componentStorage) {
            index.erase(entity->id);
        }
    }

    template <typename T, typename... Args> void addComponent(Entity& entity, Args&&... args) {
        componentIndexInternal(typeid(T)).insert({entity.id, entity.addComponent<T>(std::forward<Args>(args)...)});
    }

    template <typename T> void addComponent(Entity& entity, std::shared_ptr<T> component) {
        componentIndexInternal(typeid(T)).insert({entity.id, component});
    }

    void clear() {
        entities.clear();
        componentStorage.clear();
    }

    // Queue an entity for addition
    void queueAdd(const std::shared_ptr<Entity>& entity) { additions.emplace_back(entity); }
    // Queue an entity for removal
    void queueRemove(const std::shared_ptr<Entity>& entity) { removals.emplace_back(entity); }
    // Flush pending queues
    void flushQueue() {
        for (auto& e : additions) {
            add(e);
        }
        for (auto& e : removals) {
            remove(e);
        }
        additions.clear();
        removals.clear();
    }

    template <typename T> void removeComponent(const std::shared_ptr<Entity>& entity) {
        if (entity->hasComponent<T>()) {
            entity->removeComponent<T>();
            componentIndex<T>().erase(entity->id);
        }
    }

    template <typename T> void removeComponent(Entity& entity) {
        if (entity.hasComponent<T>()) {
            entity.removeComponent<T>();
            componentIndex<T>().erase(entity.id);
        }
    }

    // Get all entities
    std::vector<std::shared_ptr<Entity>>& all() { return entities; }

    // Iterator support
    auto begin() { return entities.begin(); }

    auto end() { return entities.end(); }

    bool empty() const { return entities.empty(); }

    auto begin() const { return entities.begin(); }

    auto end() const { return entities.end(); }

    // Lazy iterator
    template <typename... Components> FilteredView<Components...> filtered() const {
        return FilteredView<Components...>(entities);
    }

    // Find the first entity with a specific component
    template <typename T> std::shared_ptr<T> findFirstComponent() {
        auto& index = componentIndex<T>();
        if (!index.empty()) {
            return std::static_pointer_cast<T>(index.begin()->second);
        }
        return nullptr;
    }

    // Find all components in all entities
    template <typename T> std::vector<std::shared_ptr<T>> findAllComponents() {
        std::vector<std::shared_ptr<T>> result;
        auto& index = componentIndex<T>();
        for (auto& entry : index) {
            result.push_back(std::static_pointer_cast<T>(entry.second));
        }
        return result;
    }

    template <typename Component> std::shared_ptr<Entity> findEntityWithComponent() {
        for (auto& entity : *this) {
            if (entity->hasComponent<Component>()) {
                return entity;
            }
        }
        return nullptr;
    }

    // Find entities by a combination of components
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

    // entities awaiting addition
    std::vector<std::shared_ptr<Entity>> additions;
    // entities awaiting removal
    std::vector<std::shared_ptr<Entity>> removals;
};
