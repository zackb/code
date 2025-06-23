#pragma once

#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ecs {

    using Entity = uint32_t;

    template <typename T> class ComponentStorage {
    public:
        void add(Entity entity, const T& component) {
            if (entityToIndex.find(entity) != entityToIndex.end()) {
                return;
            }

            size_t index = components.size();
            components.push_back(component);
            entities.push_back(entity);
            entityToIndex[entity] = index;
        }

        void remove(Entity entity) {
            auto it = entityToIndex.find(entity);
            if (it == entityToIndex.end()) {
                return;
            }

            size_t index = it->second;
            size_t last = components.size() - 1;

            // Swap with last and pop
            components[index] = std::move(components[last]);
            entities[index] = entities[last];
            entityToIndex[entities[index]] = index;

            components.pop_back();
            entities.pop_back();
            entityToIndex.erase(entity);
        }

        T* get(Entity entity) {
            auto it = entityToIndex.find(entity);
            if (it != entityToIndex.end()) {
                return &components[it->second];
            }
            return nullptr;
        }

        const std::vector<T>& getComponents() const { return components; }
        const std::vector<Entity>& getEntities() const { return entities; }

    private:
        std::vector<T> components;
        std::vector<Entity> entities;
        std::unordered_map<Entity, size_t> entityToIndex;
    };

    class EntityManager {
    public:
        Entity createEntity() {
            Entity e = nextEntity++;
            entities.insert(e);
            return e;
        }

        void destroyEntity(Entity entity) {
            entities.erase(entity);
            for (auto& [_, storage] : componentStores) {
                storage->remove(entity);
            }
        }

        template <typename T> void addComponent(Entity entity, const T& component) {
            getStorage<T>().add(entity, component);
        }

        template <typename T> T* getComponent(Entity entity) { return getStorage<T>().get(entity); }

        template <typename T> const std::vector<T>& getComponents() { return getStorage<T>().getComponents(); }

        template <typename T> const std::vector<Entity>& getEntitiesWithComponent() {
            return getStorage<T>().getEntities();
        }

    private:
        Entity nextEntity = 0;
        std::unordered_set<Entity> entities;

        struct Storage {
            virtual void remove(Entity) = 0;
            virtual ~Storage() = default;
        };

        template <typename T> struct TypedStorage : Storage {
            ComponentStorage<T> store;

            void remove(Entity e) override { store.remove(e); }
        };

        std::unordered_map<std::type_index, std::unique_ptr<Storage>> componentStores;

        template <typename T> ComponentStorage<T>& getStorage() {
            std::type_index ti(typeid(T));
            if (componentStores.find(ti) == componentStores.end()) {
                componentStores[ti] = std::make_unique<TypedStorage<T>>();
            }
            return static_cast<TypedStorage<T>*>(componentStores[ti].get())->store;
        }
    };

}; // namespace ecs
