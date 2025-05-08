#pragma once

#include <iostream>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

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

private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> components;
};
