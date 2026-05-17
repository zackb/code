#include "world.hpp"
#include "debug.hpp"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <raymath.h>

using json = nlohmann::json;

const char* vsTerrain = R"(
#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;

out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;
out vec3 fragPosition;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize(vec3(matModel * vec4(vertexNormal, 0.0)));
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)";

const char* fsTerrain = R"(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 lightDir;

out vec4 finalColor;

void main()
{
    // Height-based color blending
    vec3 colorSand = vec3(0.8, 0.7, 0.5);
    vec3 colorGrass = vec3(0.2, 0.5, 0.2);
    vec3 colorRock = vec3(0.4, 0.4, 0.4);
    vec3 colorSnow = vec3(0.9, 0.95, 1.0);

    // relative height since world Y goes up based on local chunk
    float h = fragPosition.y;
    vec3 terrainColor;

    if (h < 5.0) {
        terrainColor = mix(colorSand, colorGrass, smoothstep(2.0, 5.0, h));
    } else if (h < 20.0) {
        terrainColor = mix(colorGrass, colorRock, smoothstep(12.0, 20.0, h));
    } else {
        terrainColor = mix(colorRock, colorSnow, smoothstep(25.0, 32.0, h));
    }

    // Basic diffuse lighting
    vec3 L = normalize(lightDir);
    vec3 N = normalize(fragNormal);
    float NdotL = max(dot(N, L), 0.0);

    vec3 ambient = vec3(0.2, 0.2, 0.25);
    vec3 diffuse = vec3(1.0, 0.95, 0.8) * NdotL; // Warm sunlight

    vec3 lighting = ambient + diffuse;

    finalColor = vec4(terrainColor * lighting, 1.0);
}
)";

World::World(const std::string& mapPath) {
    baseOffsetX = rand() % 1000;
    baseOffsetY = rand() % 1000;
    terrainScale = 4.0f;
    bool loadRandomProps = true;

    std::ifstream file(mapPath);
    if (file.is_open()) {
        try {
            json j = json::parse(file);
            if (j.contains("terrain")) {
                baseOffsetX = j["terrain"].value("offsetX", baseOffsetX);
                baseOffsetY = j["terrain"].value("offsetY", baseOffsetY);
                terrainScale = j["terrain"].value("scale", terrainScale);
            }
            if (j.contains("props")) {
                loadRandomProps = false;
                for (const auto& p : j["props"]) {
                    Prop prop;
                    prop.position = {
                        p["position"][0].get<float>(), p["position"][1].get<float>(), p["position"][2].get<float>()};
                    prop.size = {p["size"][0].get<float>(), p["size"][1].get<float>(), p["size"][2].get<float>()};
                    prop.color = {(unsigned char)p["color"][0].get<int>(),
                                  (unsigned char)p["color"][1].get<int>(),
                                  (unsigned char)p["color"][2].get<int>(),
                                  (unsigned char)p["color"][3].get<int>()};
                    globalProps.push_back(prop);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse map JSON: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Could not open map file: " << mapPath << std::endl;
    }

    // load custom shader
    terrainShader = LoadShaderFromMemory(vsTerrain, fsTerrain);
    terrainShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(terrainShader, "matModel");

    // setup light direction for shader
    int lightDirLoc = GetShaderLocation(terrainShader, "lightDir");
    Vector3 lightDir = {-1.0f, 1.0f, -1.0f}; // light pointing down, left, and backward
    SetShaderValue(terrainShader, lightDirLoc, &lightDir, SHADER_UNIFORM_VEC3);

    // initial load will happen in first Update call
}

World::~World() {
    for (auto& pair : activeChunks) {
        UnloadModel(pair.second->model);
        UnloadTexture(pair.second->texture);
    }
    activeChunks.clear();
    UnloadShader(terrainShader);
}

Mesh GenSmoothMeshHeightmap(const std::vector<float>& heights,
                            int width,
                            int height,
                            float sizeX,
                            float sizeZ,
                            const std::vector<float>& paddedHeights,
                            int paddedRes,
                            int pad) {
    Mesh mesh = {0};
    mesh.vertexCount = width * height;
    mesh.triangleCount = (width - 1) * (height - 1) * 2;

    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.indices = (unsigned short*)MemAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

    float spacingX = sizeX / (width - 1);
    float spacingZ = sizeZ / (height - 1);

    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {
            int vIdx = z * width + x;
            mesh.vertices[vIdx * 3 + 0] = x * spacingX;
            mesh.vertices[vIdx * 3 + 1] = heights[vIdx];
            mesh.vertices[vIdx * 3 + 2] = z * spacingZ;

            mesh.texcoords[vIdx * 2 + 0] = (float)x / (width - 1);
            mesh.texcoords[vIdx * 2 + 1] = (float)z / (height - 1);

            int px = x + pad;
            int pz = z + pad;
            float hL = paddedHeights[pz * paddedRes + (px - 1)];
            float hR = paddedHeights[pz * paddedRes + (px + 1)];
            float hU = paddedHeights[(pz - 1) * paddedRes + px];
            float hD = paddedHeights[(pz + 1) * paddedRes + px];

            Vector3 dx = {2.0f * spacingX, hR - hL, 0.0f};
            Vector3 dz = {0.0f, hD - hU, 2.0f * spacingZ};

            Vector3 n = Vector3Normalize(Vector3CrossProduct(dz, dx));
            if (n.y < 0.0f)
                n = Vector3Normalize(Vector3CrossProduct(dx, dz));

            mesh.normals[vIdx * 3 + 0] = n.x;
            mesh.normals[vIdx * 3 + 1] = n.y;
            mesh.normals[vIdx * 3 + 2] = n.z;
        }
    }

    int tIdx = 0;
    for (int z = 0; z < height - 1; z++) {
        for (int x = 0; x < width - 1; x++) {
            unsigned short v0 = z * width + x;
            unsigned short v1 = v0 + 1;
            unsigned short v2 = (z + 1) * width + x;
            unsigned short v3 = v2 + 1;

            mesh.indices[tIdx++] = v2;
            mesh.indices[tIdx++] = v1;
            mesh.indices[tIdx++] = v0;

            mesh.indices[tIdx++] = v1;
            mesh.indices[tIdx++] = v2;
            mesh.indices[tIdx++] = v3;
        }
    }

    return mesh;
}

std::unique_ptr<ChunkData> World::GenerateChunkData(int cx, int cz) {
    auto data = std::make_unique<ChunkData>();
    data->cx = cx;
    data->cz = cz;

    int pad = 4;
    int paddedRes = CHUNK_RES + 2 * pad;
    float adjustedScale = terrainScale * ((float)paddedRes / (float)CHUNK_RES);

    int offsetX = baseOffsetX + cx * (CHUNK_RES - 1) - pad;
    int offsetY = baseOffsetY + cz * (CHUNK_RES - 1) - pad;

    Image noiseImage = GenImagePerlinNoise(paddedRes, paddedRes, offsetX, offsetY, adjustedScale);
    // ImageBlurGaussian(&noiseImage, 2);

    // save padded heights for perfect normals across boundaries
    Color* paddedPixels = LoadImageColors(noiseImage);
    std::vector<float> paddedHeights(paddedRes * paddedRes);
    for (int i = 0; i < paddedRes * paddedRes; i++) {
        paddedHeights[i] = (paddedPixels[i].r / 255.0f) * 40.0f;
    }
    UnloadImageColors(paddedPixels);

    Rectangle cropRec = {(float)pad, (float)pad, (float)CHUNK_RES, (float)CHUNK_RES};
    ImageCrop(&noiseImage, cropRec);

    Color* pixels = LoadImageColors(noiseImage);
    data->heights.resize(CHUNK_RES * CHUNK_RES);
    for (int i = 0; i < CHUNK_RES * CHUNK_RES; i++) {
        data->heights[i] = (pixels[i].r / 255.0f) * 40.0f;
    }
    UnloadImageColors(pixels);

    data->terrainMesh = GenSmoothMeshHeightmap(
        data->heights, CHUNK_RES, CHUNK_RES, CHUNK_SIZE, CHUNK_SIZE, paddedHeights, paddedRes, pad);
    UnloadImage(noiseImage);

    data->colorImage = GenImageColor(CHUNK_RES, CHUNK_RES, WHITE);

    float minX = cx * CHUNK_SIZE;
    float maxX = cx * CHUNK_SIZE + CHUNK_SIZE;
    float minZ = cz * CHUNK_SIZE;
    float maxZ = cz * CHUNK_SIZE + CHUNK_SIZE;

    for (const auto& prop : globalProps) {
        if (prop.position.x >= minX && prop.position.x < maxX && prop.position.z >= minZ && prop.position.z < maxZ) {

            Prop p = prop;
            float localX = p.position.x - (cx * CHUNK_SIZE);
            float localZ = p.position.z - (cz * CHUNK_SIZE);

            float px = localX / (CHUNK_SIZE / (CHUNK_RES - 1.0f));
            float pz = localZ / (CHUNK_SIZE / (CHUNK_RES - 1.0f));

            int ix = (int)px;
            int iz = (int)pz;
            if (ix < 0)
                ix = 0;
            if (ix >= CHUNK_RES - 1)
                ix = CHUNK_RES - 2;
            if (iz < 0)
                iz = 0;
            if (iz >= CHUNK_RES - 1)
                iz = CHUNK_RES - 2;

            float fx = px - ix;
            float fz = pz - iz;

            float h00 = data->heights[iz * CHUNK_RES + ix];
            float h10 = data->heights[iz * CHUNK_RES + ix + 1];
            float h01 = data->heights[(iz + 1) * CHUNK_RES + ix];
            float h11 = data->heights[(iz + 1) * CHUNK_RES + ix + 1];

            float height = 0.0f;
            if (fx + fz <= 1.0f) {
                height = h00 + fx * (h10 - h00) + fz * (h01 - h00);
            } else {
                height = h11 + (1.0f - fx) * (h01 - h11) + (1.0f - fz) * (h10 - h11);
            }

            p.position.y = height + (p.size.y / 2.0f);
            data->props.push_back(p);
        }
    }

    return data;
}

void World::UnloadChunk(int cx, int cz) {
    auto it = activeChunks.find({cx, cz});
    if (it != activeChunks.end()) {
        UnloadModel(it->second->model);
        UnloadTexture(it->second->texture);
        activeChunks.erase(it);
    }
}

void World::Update(const Vector3& playerPos, float dt) {
    // process loaded chunks on the main thread
    for (auto it = loadingChunks.begin(); it != loadingChunks.end();) {
        if (it->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            auto data = it->get();

            // GenMeshHeightmap calls UploadMesh internally, which silently fails on background thread
            // because there is no OpenGL context. It leaves vboId allocated but invalid.
            // We must free the leaked vboId array and call UploadMesh again on the main thread!
            if (data->terrainMesh.vboId != nullptr) {
                MemFree(data->terrainMesh.vboId);
                data->terrainMesh.vboId = nullptr;
            }
            UploadMesh(&data->terrainMesh, false);

            auto chunk = std::make_unique<Chunk>();
            chunk->cx = data->cx;
            chunk->cz = data->cz;
            chunk->model = LoadModelFromMesh(data->terrainMesh);
            chunk->texture = LoadTextureFromImage(data->colorImage);
            chunk->model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = chunk->texture;
            chunk->model.materials[0].shader = terrainShader;
            chunk->props = std::move(data->props);
            chunk->heights = std::move(data->heights);

            UnloadImage(data->colorImage);

            activeChunks[{chunk->cx, chunk->cz}] = std::move(chunk);
            loadingChunkCoords.erase({data->cx, data->cz});
            it = loadingChunks.erase(it);
        } else {
            ++it;
        }
    }

    int pcx = std::floor(playerPos.x / CHUNK_SIZE);
    int pcz = std::floor(playerPos.z / CHUNK_SIZE);

    // load new chunks in radius
    for (int x = -CHUNK_LOAD_RADIUS; x <= CHUNK_LOAD_RADIUS; ++x) {
        for (int z = -CHUNK_LOAD_RADIUS; z <= CHUNK_LOAD_RADIUS; ++z) {
            int cx = pcx + x;
            int cz = pcz + z;
            if (activeChunks.find({cx, cz}) == activeChunks.end() &&
                loadingChunkCoords.find({cx, cz}) == loadingChunkCoords.end()) {

                loadingChunkCoords.insert({cx, cz});
                loadingChunks.push_back(
                    std::async(std::launch::async, [this, cx, cz]() { return GenerateChunkData(cx, cz); }));
            }
        }
    }

    // unload old chunks
    std::vector<std::pair<int, int>> toUnload;
    for (const auto& pair : activeChunks) {
        int cx = pair.first.first;
        int cz = pair.first.second;
        if (std::abs(cx - pcx) > CHUNK_LOAD_RADIUS || std::abs(cz - pcz) > CHUNK_LOAD_RADIUS) {
            toUnload.push_back({cx, cz});
        }
    }

    for (const auto& key : toUnload) {
        UnloadChunk(key.first, key.second);
    }
}

float World::GetTerrainHeightAt(float x, float z) const {
    int cx = std::floor(x / CHUNK_SIZE);
    int cz = std::floor(z / CHUNK_SIZE);

    auto tryChunk = [&](int cX, int cZ) -> float {
        auto it = activeChunks.find({cX, cZ});
        if (it != activeChunks.end()) {
            const Chunk* chunk = it->second.get();
            float localX = x - (cX * CHUNK_SIZE);
            float localZ = z - (cZ * CHUNK_SIZE);

            // bounds check
            if (localX < 0.0f || localX > CHUNK_SIZE || localZ < 0.0f || localZ > CHUNK_SIZE) {
                return -10000.0f;
            }

            float px = localX / (CHUNK_SIZE / (CHUNK_RES - 1.0f));
            float pz = localZ / (CHUNK_SIZE / (CHUNK_RES - 1.0f));

            int ix = (int)px;
            int iz = (int)pz;
            if (ix < 0)
                ix = 0;
            if (ix >= CHUNK_RES - 1)
                ix = CHUNK_RES - 2;
            if (iz < 0)
                iz = 0;
            if (iz >= CHUNK_RES - 1)
                iz = CHUNK_RES - 2;

            float fx = px - ix;
            float fz = pz - iz;

            float h00 = chunk->heights[iz * CHUNK_RES + ix];
            float h10 = chunk->heights[iz * CHUNK_RES + ix + 1];
            float h01 = chunk->heights[(iz + 1) * CHUNK_RES + ix];
            float h11 = chunk->heights[(iz + 1) * CHUNK_RES + ix + 1];

            if (fx + fz <= 1.0f) {
                return h00 + fx * (h10 - h00) + fz * (h01 - h00);
            } else {
                return h11 + (1.0f - fx) * (h01 - h11) + (1.0f - fz) * (h10 - h11);
            }
        }
        return -10000.0f; // invalid
    };

    float y = tryChunk(cx, cz);
    float y_calc = 0.0f;
    if (y != -10000.0f)
        y_calc = y;
    else {
        int nextCx = (x > cx * CHUNK_SIZE) ? cx + 1 : cx - 1;
        y = tryChunk(nextCx, cz);
        if (y != -10000.0f)
            y_calc = y;
        else {
            int nextCz = (z > cz * CHUNK_SIZE) ? cz + 1 : cz - 1;
            y = tryChunk(cx, nextCz);
            if (y != -10000.0f)
                y_calc = y;
            else {
                y = tryChunk(nextCx, nextCz);
                if (y != -10000.0f)
                    y_calc = y;
            }
        }
    }

    return y_calc;
}

void World::Draw() const {
    if (debug::enabled) {
        return;
    }
    static int draw_frames = 0;
    bool should_print = (draw_frames++ % 60 == 0);

    if (should_print) {
        printf("Drawing World. Active chunks: %zu\n", activeChunks.size());
    }

    for (const auto& pair : activeChunks) {
        const auto& chunk = pair.second;
        Vector3 chunkPos = {(float)(chunk->cx * CHUNK_SIZE), 0.0f, (float)(chunk->cz * CHUNK_SIZE)};
        DrawModel(chunk->model, chunkPos, 1.0f, WHITE);

        for (const auto& prop : chunk->props) {
            DrawCubeV(prop.position, prop.size, prop.color);
            DrawCubeWiresV(prop.position, prop.size, BLACK);
        }
    }
}
