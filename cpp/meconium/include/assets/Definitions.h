#pragma once
#include "../components/TileMap.h"
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
        if (str == "solid") return TileType::Solid;
        if (str == "ramp_left") return TileType::RampLeft;
        if (str == "ramp_right") return TileType::RampRight;
        return TileType::Empty;
    }
}

inline void from_json(const nlohmann::json& j, TileDefinition& tile) {
    tile.id = j.at("id").get<int>();
    tile.type = tileTypeFromString(j.at("type").get<std::string>());
}

inline void from_json(const nlohmann::json& j, TileSheetDefinition& def) {
    def.texture = j.at("texture").get<std::string>();
    def.tileWidth = j.at("tileWidth").get<int>();
    def.tileHeight = j.at("tileHeight").get<int>();
    def.tiles = j.at("tiles").get<std::vector<TileDefinition>>();
}

inline void from_json(const nlohmann::json& j, AnimationDefinition& anim) {
    anim.name = j.at("name").get<std::string>();
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