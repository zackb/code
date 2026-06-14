#include "resources.hpp"
#include <cstring>
#include <iostream>

ResourceCache::~ResourceCache() {
    for (auto& e : entries) {
        UnloadModel(e.model);
        if (e.anims.anims != nullptr) {
            UnloadModelAnimations(e.anims.anims, e.anims.count);
        }
    }
}

AnimSet ResourceCache::loadAnimSet(const std::string& path) const {
    AnimSet set;
    set.anims = LoadModelAnimations(path.c_str(), &set.count);

    for (int i = 0; i < set.count; i++) {
        const char* name = set.anims[i].name;
        if (strstr(name, "Taunt") != nullptr)
            set.idleIndex = i;
        else if (strstr(name, "Walk") != nullptr)
            set.walkIndex = i;
        else if (strstr(name, "Run") != nullptr)
            set.runIndex = i;
        else if (strstr(name, "Jump") != nullptr)
            set.jumpIndex = i;
    }
    return set;
}

ModelHandle ResourceCache::loadStatic(const std::string& path) {
    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i].shared && entries[i].path == path) {
            return static_cast<ModelHandle>(i);
        }
    }

    Entry e;
    e.path = path;
    e.shared = true;
    e.model = LoadModel(path.c_str());
    entries.push_back(e);
    return static_cast<ModelHandle>(entries.size() - 1);
}

ModelHandle ResourceCache::loadAnimated(const std::string& path) {
    Entry e;
    e.path = path;
    e.shared = false;
    e.model = LoadModel(path.c_str());
    e.anims = loadAnimSet(path);

    std::cout << "Loaded animated model '" << path << "' with " << e.anims.count << " animations" << std::endl;

    entries.push_back(e);
    return static_cast<ModelHandle>(entries.size() - 1);
}

Model& ResourceCache::model(ModelHandle h) { return entries[h].model; }

const AnimSet& ResourceCache::anims(ModelHandle h) const { return entries[h].anims; }
