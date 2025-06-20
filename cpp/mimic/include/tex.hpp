#pragma once

#include <raylib.h>

namespace tex {

    Mesh GenTexturedCube(float width, float height, float length);

    void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

    // Draw cube with texture piece applied to all faces
    void DrawCubeTextureRec(
        Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color);

    Model MakeWallModel(Texture2D texture, float width, float height, float tileSize = 1.0);
    Model MakeVerticalWallModel(Texture2D texture, float width, float height, float tileSize);
} // namespace tex
