#include "ResourceManager.h"

std::unordered_map<std::string, SDL_Texture *> ResourceManager::textures;

std::shared_ptr<Sprite> ResourceManager::loadSprite(const std::string& path, int frameWidth, int frameHeight) {
    SDL_Texture* texture = loadTexture(path);
    if (!texture) {
        return nullptr;
    }
    return std::make_shared<Sprite>(texture, frameWidth, frameHeight);
}

std::shared_ptr<Sprite> ResourceManager::loadSpriteScaled(const std::string& filePath, int maxWidth, int maxHeight) {
    SDL_Texture *tex = ResourceManager::loadTexture(filePath);
    if (!tex) {
        std::cerr << "failed to load texture for sprite" << std::endl;
        return nullptr;
    }

    auto sprite = std::make_shared<Sprite>();
    sprite->texture = tex;
    SDL_QueryTexture(tex, nullptr, nullptr, &sprite->width, &sprite->height);

    // Get original image dimensions
    int originalWidth = sprite->width;
    int originalHeight = sprite->height;

    // Calculate scale factor to fit within the bounds
    float scaleX = static_cast<float>(maxWidth) / originalWidth;
    float scaleY = static_cast<float>(maxHeight) / originalHeight;

    // Use the smaller scale factor to ensure the image fits within both dimensions
    float scaleFactor = std::min(scaleX, scaleY);

    // Calculate new width and height based on the scale factor
    sprite->width = static_cast<int>(originalWidth * scaleFactor);
    sprite->height = static_cast<int>(originalHeight * scaleFactor);

    return sprite;
}

SDL_Texture* ResourceManager::loadTexture(const std::string& filePath) {
    // Check if we already loaded this texture
    if (const auto it = textures.find(filePath); it != textures.end()) {
        return it->second;
    }

    if (!Context::renderer) {
        std::cerr << "Renderer is NULL before loadTexture!\n";
    }

    SDL_Surface *surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(Context::renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
    }
    // Cache the texture
    textures[filePath] = texture;

    return texture;
}

std::shared_ptr<AnimationComponent> ResourceManager::createPlayerAnimations(
    const std::string& spriteSheetPath, const int frameWidth,
    const int frameHeight) {
    SDL_Texture *texture = loadTexture(spriteSheetPath);
    if (!texture) {
        return nullptr;
    }

    auto animComponent = std::make_shared<AnimationComponent>();

    // Create idle animation (example: first row in sprite sheet)
    auto idleAnim = createAnimation("idle", true);
    idleAnim->addFrame({0, 0, frameWidth, frameHeight}, 200);
    idleAnim->addFrame({frameWidth, 0, frameWidth, frameHeight}, 200);
    idleAnim->addFrame({frameWidth * 2, 0, frameWidth, frameHeight}, 200);
    idleAnim->addFrame({frameWidth * 3, 0, frameWidth, frameHeight}, 200);
    animComponent->addAnimation(AnimationState::IDLE, idleAnim);

    // Create walking animation (example: second row in sprite sheet)
    auto walkAnim = createAnimation("walk", true);
    walkAnim->addFrame({0, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth * 2, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth * 3, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth * 4, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth * 5, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth * 6, frameHeight, frameWidth, frameHeight}, 100);
    walkAnim->addFrame({frameWidth * 7, frameHeight, frameWidth, frameHeight}, 100);
    animComponent->addAnimation(AnimationState::WALKING, walkAnim);

    // Create jumping animation (example: third row in sprite sheet)
    auto jumpAnim = createAnimation("jump", false);
    jumpAnim->addFrame({0, frameHeight * 2, frameWidth, frameHeight}, 150);
    jumpAnim->addFrame({frameWidth, frameHeight * 2, frameWidth, frameHeight}, 150);
    jumpAnim->addFrame({frameWidth * 2, frameHeight * 2, frameWidth, frameHeight}, 150);
    jumpAnim->addFrame({frameWidth * 3, frameHeight * 2, frameWidth, frameHeight}, 150);
    jumpAnim->addFrame({frameWidth * 4, frameHeight * 2, frameWidth, frameHeight}, 150);
    jumpAnim->addFrame({frameWidth * 5, frameHeight * 2, frameWidth, frameHeight}, 150);
    animComponent->addAnimation(AnimationState::JUMPING, jumpAnim);

    // Create falling animation (example: fourth row in sprite sheet)
    // TODO: this should be row 4 (frameHeight * 3)
    auto fallAnim = createAnimation("fall", true);
    fallAnim->addFrame({0, frameHeight * 2, frameWidth, frameHeight}, 150);
    fallAnim->addFrame({frameWidth, frameHeight * 2, frameWidth, frameHeight}, 150);
    animComponent->addAnimation(AnimationState::FALLING, fallAnim);

    return animComponent;
}

void ResourceManager::cleanup() {
    for (auto &pair: textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}
