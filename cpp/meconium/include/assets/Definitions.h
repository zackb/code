#pragma once
#include "json.hpp"

#include <string>

// JSON model

// Spritesheet
struct AnimationDefinition {
    std::string name;
    int row;
    int frameCount;
    bool looping;
    int duration;
};

struct SpriteSheetDefinition {
    std::string texture;
    int tileWidth;
    int tileHeight;
    std::vector<AnimationDefinition> animations;
};

// Tileset
enum class TileType { Empty, Solid, RampLeft, RampRight };

struct TileDefinition {
    int id;
    TileType type;
};

struct TileSetDefinition {
    std::string texture;
    int tileWidth;
    int tileHeight;
    std::vector<TileDefinition> tiles;
};

// Map
struct MapDefinition {
    std::string name;
    std::string tilesheet;
    std::string tilemapCSV;
    std::string background;
    int backgroundLayers;
};

// Background
struct BackgroundLayerDefinition {
    std::string texture;
    float speed;
};

struct BackgroundDefinition {
    std::vector<BackgroundLayerDefinition> layers;
};

// Level
struct LevelDefinition {
    std::string tilemap;
    std::string tileset;
    BackgroundDefinition background;
};

inline TileType tileTypeFromString(const std::string& str) {
    if (str == "solid")
        return TileType::Solid;
    if (str == "ramp_left")
        return TileType::RampLeft;
    if (str == "ramp_right")
        return TileType::RampRight;
    return TileType::Empty;
}

inline void from_json(const nlohmann::json& j, BackgroundLayerDefinition& def) {
    def.texture = j.at("texture").get<std::string>();
    def.speed = j.at("speed").get<float>();
}

inline void from_json(const nlohmann::json& j, BackgroundDefinition& def) {
    def.layers = j.at("layers").get<std::vector<BackgroundLayerDefinition>>();
}

inline void from_json(const nlohmann::json& j, LevelDefinition& def) {
    def.tilemap = j.at("tilemap").get<std::string>();
    def.tileset = j.at("tileset").get<std::string>();
    def.background = j.at("background").get<BackgroundDefinition>();
}

inline void from_json(const nlohmann::json& j, TileDefinition& tile) {
    tile.id = j.at("id").get<int>();
    tile.type = tileTypeFromString(j.at("type").get<std::string>());
}

inline void from_json(const nlohmann::json& j, TileSetDefinition& def) {
    def.texture = j.at("texture").get<std::string>();
    def.tileWidth = j.at("tileWidth").get<int>();
    def.tileHeight = j.at("tileHeight").get<int>();

    const auto& tilesJson = j.at("tiles");
    for (auto it = tilesJson.begin(); it != tilesJson.end(); ++it) {
        TileDefinition tile;
        tile.id = std::stoi(it.key());
        tile.type = tileTypeFromString(it.value().at("type").get<std::string>());
        def.tiles.push_back(tile);
    }
}

inline void from_json(const nlohmann::json& j, AnimationDefinition& anim) {
    anim.row = j.at("row").get<int>();
    anim.frameCount = j.at("frameCount").get<int>();
    anim.looping = j.at("looping").get<bool>();
    anim.duration = j.at("duration").get<int>();
}

inline void from_json(const nlohmann::json& j, SpriteSheetDefinition& sheet) {
    sheet.texture = j.at("texture").get<std::string>();
    sheet.tileWidth = j.at("tileWidth").get<int>();
    sheet.tileHeight = j.at("tileHeight").get<int>();

    // JSON object animations -> vector<AnimationDefinition>
    for (auto it = j.at("animations").begin(); it != j.at("animations").end(); ++it) {
        AnimationDefinition anim;
        anim.name = it.key();
        from_json(it.value(), anim);
        sheet.animations.push_back(anim);
    }
}

inline void from_json(const nlohmann::json& j, MapDefinition& def) {
    def.name = j.at("name").get<std::string>();
    def.tilesheet = j.at("tilesheet").get<std::string>();
    def.tilemapCSV = j.at("tilemapCSV").get<std::string>();
    def.background = j.at("background").get<std::string>();
    def.backgroundLayers = j.at("backgroundLayers").get<int>();
}