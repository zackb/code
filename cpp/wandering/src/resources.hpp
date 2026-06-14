#pragma once
#include <raylib.h>
#include <string>
#include <vector>

// Lightweight handle into the ResourceCache. -1 means "no model".
using ModelHandle = int;
constexpr ModelHandle INVALID_MODEL = -1;

// Animation set that travels with a model. For character models loaded from a
// .glb, the named clips are matched to the four states the game cares about.
struct AnimSet {
    ModelAnimation* anims = nullptr;
    int count = 0;
    int idleIndex = 0;
    int walkIndex = 0;
    int runIndex = 0;
    int jumpIndex = 0;
};

// Owns every Model/ModelAnimation loaded by the game so that GPU resources are
// unloaded exactly once (in the destructor). Components only ever hold a
// ModelHandle, never a Model by value -- copying a raylib Model and unloading
// both copies would double-free GPU buffers.
//
// Static props may be *shared*: loadStatic() de-duplicates by path so many
// entities reference one Model. Animated characters must NOT be shared, because
// UpdateModelAnimation mutates the model's bone/vertex buffers in place; two
// entities sharing one animated Model would both render the last pose written.
// loadAnimated() therefore always returns a fresh, independent instance.
class ResourceCache {
public:
    ResourceCache() = default;
    ~ResourceCache();

    ResourceCache(const ResourceCache&) = delete;
    ResourceCache& operator=(const ResourceCache&) = delete;

    // Shared static model, de-duplicated by path.
    ModelHandle loadStatic(const std::string& path);

    // Independent animated model instance (own copy of mesh + animations).
    ModelHandle loadAnimated(const std::string& path);

    Model& model(ModelHandle h);
    const AnimSet& anims(ModelHandle h) const;

private:
    struct Entry {
        std::string path;
        bool shared = false;
        Model model{};
        AnimSet anims{};
    };

    AnimSet loadAnimSet(const std::string& path) const;

    std::vector<Entry> entries;
};
