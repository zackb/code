#pragma once
#include <map>
#include <memory>
#include <raylib.h>
#include <vector>

struct Prop {
    Vector3 position;
    Color color;
    Vector3 size;
};

struct Chunk {
    int cx, cz;
    Model model;
    Texture2D texture;
    std::vector<Prop> props;    // Props inside this chunk
    std::vector<float> heights; // Cached heights for O(1) lookups
};

#include <future>
#include <raymath.h>
#include <set>

struct ChunkData {
    int cx, cz;
    Mesh terrainMesh;
    Image colorImage;
    std::vector<Prop> props;
    std::vector<float> heights;
};

class World {
public:
    World(const std::string& mapPath);
    ~World();

    void Update(const Vector3& playerPos, float dt);
    void Draw() const;

    float GetTerrainHeightAt(float x, float z) const;

    // True once the chunk containing (x, z) has finished generating, so its
    // heights can be sampled. Used to defer placing static scenery until the
    // ground beneath it exists.
    bool IsLoadedAt(float x, float z) const;

private:
    std::unique_ptr<ChunkData> GenerateChunkData(int cx, int cz);
    void UnloadChunk(int cx, int cz);

    std::map<std::pair<int, int>, std::unique_ptr<Chunk>> activeChunks;
    std::set<std::pair<int, int>> loadingChunkCoords;
    std::vector<std::future<std::unique_ptr<ChunkData>>> loadingChunks;

    std::vector<Prop> globalProps; // all props loaded from map
    Shader terrainShader;
    Texture2D terrainTextures[4]; // sand, grass, rock, snow splatmap layers
    int terrainTexLocs[4];        // shader sampler locations for the above
    Model sphereModel;
    float sphereRotation;

    int baseOffsetX;
    int baseOffsetY;
    float terrainScale;

    const float CHUNK_SIZE = 400.0f;
    const int CHUNK_RES = 256;
    const int CHUNK_LOAD_RADIUS = 1; // 3x3 grid
};
