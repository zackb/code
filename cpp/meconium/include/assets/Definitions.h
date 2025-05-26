#pragma once
#include "components/EnemyAI.h"
#include "json.hpp"

#include <string>

// JSON model

struct Rect {
    int x;
    int y;
    int width;
    int height;
};

// Spritesheet
struct AnimationDefinition {
    std::string name;
    int row;
    int rowOffset;
    int frameCount;
    bool looping;
    int duration;
};

struct SpriteSheetDefinition {
    std::string texture;
    int tileWidth;
    int tileHeight;
    float scale;
    std::optional<Rect> collider;
    std::vector<AnimationDefinition> animations;
    int speed;
    int lifetimeMs;
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
    float scale;
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

// Enemy
struct PatrolDefinition {
    int left;
    int right;
    float speed;
};

struct ChaseDefinition {
    float speed;
};

struct AttackDefinition {
    std::string type;
    std::string sprite;
    int cooldownMs;
    int damage;
    int range;
    std::string sound;
};

struct EnemyDefinition {
    std::string type;
    std::string sprite;
    int x;
    int y;
    int triggerX;
    EnemyBehavior behavior;
    std::optional<PatrolDefinition> patrol;
    std::optional<ChaseDefinition> chase;
    std::optional<AttackDefinition> attack;
};

// Pickup
struct PickupDefinition {
    std::string type;
    std::string sprite;
    int amount;
    int x;
    int y;
};

// Player
struct PlayerDefinition {
    std::string sprite;
    AttackDefinition attack;
    std::optional<Rect> hitbox;
};

// Level
struct LevelDefinition {
    std::string tilemap;
    std::string tileset;
    std::string backgroundMusic;
    BackgroundDefinition background;
    std::vector<EnemyDefinition> enemies;
    std::vector<PickupDefinition> pickups;
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

inline void from_json(const nlohmann::json& j, Rect& rect) {
    rect.x = j.at("x").get<int>();
    rect.y = j.at("y").get<int>();
    rect.width = j.at("width").get<int>();
    rect.height = j.at("height").get<int>();
}

inline void from_json(const nlohmann::json& j, BackgroundLayerDefinition& def) {
    def.texture = j.at("texture").get<std::string>();
    def.speed = j.at("speed").get<float>();
}

inline void from_json(const nlohmann::json& j, BackgroundDefinition& def) {
    def.layers = j.at("layers").get<std::vector<BackgroundLayerDefinition>>();
}

inline void from_json(const nlohmann::json& j, PatrolDefinition& p) {
    j.at("left").get_to(p.left);
    j.at("right").get_to(p.right);
    j.at("speed").get_to(p.speed);
}

inline void from_json(const nlohmann::json& j, ChaseDefinition& c) { j.at("speed").get_to(c.speed); }

inline void from_json(const nlohmann::json& j, AttackDefinition& def) {
    def.type = j.at("type").get<std::string>();
    def.cooldownMs = j.at("cooldownMs").get<int>();
    def.damage = j.at("damage").get<int>();
    if (j.contains("range")) {
        def.range = j.at("range").get<int>();
    }
    if (j.contains("sprite")) {
        def.sprite = j.at("sprite").get<std::string>();
    }
    if (j.contains("sound")) {
        def.sound = j.at("sound").get<std::string>();
    }
}

inline void from_json(const nlohmann::json& j, EnemyDefinition& e) {
    e.type = j.at("type").get<std::string>();
    e.sprite = j.at("sprite").get<std::string>();
    e.x = j.at("x").get<int>();
    e.y = j.at("y").get<int>();
    e.triggerX = j.at("triggerX").get<int>();
    auto bs = j.at("behavior").get<std::string>();

    if (bs == "idle") {
        e.behavior = EnemyBehavior::IDLE;
    } else if (bs == "patrol") {
        e.behavior = EnemyBehavior::PATROL;
    } else if (bs == "chase") {
        e.behavior = EnemyBehavior::CHASE;
    } else {
        throw std::runtime_error("Unknown enemy behavior: " + bs);
    }

    if (j.contains("patrol") && !j.at("patrol").is_null()) {
        e.patrol = j.at("patrol").get<PatrolDefinition>();
    } else {
        e.patrol = std::nullopt;
    }

    if (j.contains("chase") && !j.at("chase").is_null()) {
        e.chase = j.at("chase").get<ChaseDefinition>();
    } else {
        e.chase = std::nullopt;
    }

    if (j.contains("attack") && !j.at("attack").is_null()) {
        e.attack = j.at("attack").get<AttackDefinition>();
    } else {
        e.attack = std::nullopt;
    }
}

inline void from_json(const nlohmann::json& j, PickupDefinition& p) {
    p.type = j.at("type").get<std::string>();
    p.sprite = j.at("sprite").get<std::string>();
    if (j.contains("amount")) {
        p.amount = j.at("amount").get<int>();
    }
    p.x = j.at("x").get<int>();
    p.y = j.at("y").get<int>();
}

inline void from_json(const nlohmann::json& j, LevelDefinition& def) {
    def.tilemap = j.at("tilemap").get<std::string>();
    def.tileset = j.at("tileset").get<std::string>();
    if (j.contains("backgroundMusic")) {
        def.backgroundMusic = j.at("backgroundMusic").get<std::string>();
    }
    def.tileset = j.at("tileset").get<std::string>();
    def.background = j.at("background").get<BackgroundDefinition>();
    def.enemies = j.value("enemies", std::vector<EnemyDefinition>{});
    def.pickups = j.value("pickups", std::vector<PickupDefinition>{});
}

inline void from_json(const nlohmann::json& j, TileDefinition& tile) {
    tile.id = j.at("id").get<int>();
    tile.type = tileTypeFromString(j.at("type").get<std::string>());
}

inline void from_json(const nlohmann::json& j, TileSetDefinition& def) {
    def.texture = j.at("texture").get<std::string>();
    def.tileWidth = j.at("tileWidth").get<int>();
    def.tileHeight = j.at("tileHeight").get<int>();
    if (j.contains("scale")) {
        def.scale = j.at("scale").get<float>();
    } else {
        def.scale = 1.0;
    }

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
    if (j.contains("rowOffset")) {
        anim.rowOffset = j.at("rowOffset").get<int>();
    } else {
        anim.rowOffset = 0;
    }
    anim.frameCount = j.at("frameCount").get<int>();
    anim.looping = j.at("looping").get<bool>();
    anim.duration = j.at("duration").get<int>();
}
inline void from_json(const nlohmann::json& j, SpriteSheetDefinition& sheet) {
    sheet.texture = j.at("texture").get<std::string>();
    sheet.tileWidth = j.at("tileWidth").get<int>();
    sheet.tileHeight = j.at("tileHeight").get<int>();
    sheet.scale = j.at("scale").get<float>();
    if (j.contains("collider") && !j.at("collider").is_null()) {
        sheet.collider = j.at("collider").get<Rect>();
    } else {
        sheet.collider = std::nullopt;
    }

    // JSON object animations -> vector<AnimationDefinition>
    if (j.contains("animations")) {
        for (auto it = j.at("animations").begin(); it != j.at("animations").end(); ++it) {
            AnimationDefinition anim;
            anim.name = it.key();
            from_json(it.value(), anim);
            sheet.animations.push_back(anim);
        }
    }
    if (j.contains("speed")) {
        sheet.speed = j.at("speed").get<int>();
    }
    if (j.contains("lifetimeMs")) {
        sheet.lifetimeMs = j.at("lifetimeMs").get<int>();
    }
}

inline void from_json(const nlohmann::json& j, MapDefinition& def) {
    def.name = j.at("name").get<std::string>();
    def.tilesheet = j.at("tilesheet").get<std::string>();
    def.tilemapCSV = j.at("tilemapCSV").get<std::string>();
    def.background = j.at("background").get<std::string>();
    def.backgroundLayers = j.at("backgroundLayers").get<int>();
}

inline void from_json(const nlohmann::json j, PlayerDefinition& def) {
    def.sprite = j.at("sprite").get<std::string>();
    def.attack = j.at("attack").get<AttackDefinition>();
    if (j.contains("hitbox")) {
        def.hitbox = j.at("hitbox").get<Rect>();
    }
}
