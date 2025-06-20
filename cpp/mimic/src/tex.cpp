#include "tex.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <raylib.h>
#include <rlgl.h>

namespace tex {

    Mesh GenTexturedCube(float width, float height, float length) {
        Mesh mesh = {0};

        float w = width / 2.0f;
        float h = height / 2.0f;
        float l = length / 2.0f;

        Vector3 vertices[36] = {
            // Front face
            {-w, -h, -l},
            {w, -h, -l},
            {w, h, -l},
            {-w, -h, -l},
            {w, h, -l},
            {-w, h, -l},

            // Back face
            {w, -h, l},
            {-w, -h, l},
            {-w, h, l},
            {w, -h, l},
            {-w, h, l},
            {w, h, l},

            // Top face
            {-w, h, -l},
            {w, h, -l},
            {w, h, l},
            {-w, h, -l},
            {w, h, l},
            {-w, h, l},

            // Bottom face
            {-w, -h, l},
            {w, -h, l},
            {w, -h, -l},
            {-w, -h, l},
            {w, -h, -l},
            {-w, -h, -l},

            // Right face
            {w, -h, -l},
            {w, -h, l},
            {w, h, l},
            {w, -h, -l},
            {w, h, l},
            {w, h, -l},

            // Left face
            {-w, -h, l},
            {-w, -h, -l},
            {-w, h, -l},
            {-w, -h, l},
            {-w, h, -l},
            {-w, h, l},
        };

        Vector3 normals[36] = {
            // Front
            {0, 0, -1},
            {0, 0, -1},
            {0, 0, -1},
            {0, 0, -1},
            {0, 0, -1},
            {0, 0, -1},

            // Back
            {0, 0, 1},
            {0, 0, 1},
            {0, 0, 1},
            {0, 0, 1},
            {0, 0, 1},
            {0, 0, 1},

            // Top
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0},
            {0, 1, 0},

            // Bottom
            {0, -1, 0},
            {0, -1, 0},
            {0, -1, 0},
            {0, -1, 0},
            {0, -1, 0},
            {0, -1, 0},

            // Right
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0},
            {1, 0, 0},

            // Left
            {-1, 0, 0},
            {-1, 0, 0},
            {-1, 0, 0},
            {-1, 0, 0},
            {-1, 0, 0},
            {-1, 0, 0},
        };

        Vector2 texcoords[36] = {
            // Repeat for each face
            {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 0}, {0, 0},

            {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 0}, {0, 0},

            {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 0}, {0, 0},

            {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 0}, {0, 0},

            {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 0}, {0, 0},

            {0, 1}, {1, 1}, {1, 0}, {0, 1}, {1, 0}, {0, 0},
        };

        mesh.vertexCount = 36;
        mesh.triangleCount = 12;

        mesh.vertices = (float*)RL_MALLOC(sizeof(Vector3) * 36);
        mesh.normals = (float*)RL_MALLOC(sizeof(Vector3) * 36);
        mesh.texcoords = (float*)RL_MALLOC(sizeof(Vector2) * 36);

        memcpy(mesh.vertices, vertices, sizeof(Vector3) * 36);
        memcpy(mesh.normals, normals, sizeof(Vector3) * 36);
        memcpy(mesh.texcoords, texcoords, sizeof(Vector2) * 36);

        UploadMesh(&mesh, false);

        return mesh;
    }

    void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color) {
        float x = position.x;
        float y = position.y;
        float z = position.z;

        // Set desired texture to be enabled while drawing following vertex data
        rlSetTexture(texture.id);

        rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);
        // Front Face
        rlNormal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Right Of The Texture and Quad
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Left Of The Texture and Quad
        // Back Face
        rlNormal3f(0.0f, 0.0f, -1.0f); // Normal Pointing Away From Viewer
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Right Of The Texture and Quad
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Right Of The Texture and Quad
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Left Of The Texture and Quad
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Left Of The Texture and Quad
        // Top Face
        rlNormal3f(0.0f, 1.0f, 0.0f); // Normal Pointing Up
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left Of The Texture and Quad
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right Of The Texture and Quad
        // Bottom Face
        rlNormal3f(0.0f, -1.0f, 0.0f); // Normal Pointing Down
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Top Right Of The Texture and Quad
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Top Left Of The Texture and Quad
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
        // Right face
        rlNormal3f(1.0f, 0.0f, 0.0f); // Normal Pointing Right
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(x + width / 2, y - height / 2, z - length / 2); // Bottom Right Of The Texture and Quad
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(x + width / 2, y + height / 2, z - length / 2); // Top Right Of The Texture and Quad
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(x + width / 2, y + height / 2, z + length / 2); // Top Left Of The Texture and Quad
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(x + width / 2, y - height / 2, z + length / 2); // Bottom Left Of The Texture and Quad
        // Left Face
        rlNormal3f(-1.0f, 0.0f, 0.0f); // Normal Pointing Left
        rlTexCoord2f(0.0f, 0.0f);
        rlVertex3f(x - width / 2, y - height / 2, z - length / 2); // Bottom Left Of The Texture and Quad
        rlTexCoord2f(1.0f, 0.0f);
        rlVertex3f(x - width / 2, y - height / 2, z + length / 2); // Bottom Right Of The Texture and Quad
        rlTexCoord2f(1.0f, 1.0f);
        rlVertex3f(x - width / 2, y + height / 2, z + length / 2); // Top Right Of The Texture and Quad
        rlTexCoord2f(0.0f, 1.0f);
        rlVertex3f(x - width / 2, y + height / 2, z - length / 2); // Top Left Of The Texture and Quad
        rlEnd();
        // rlPopMatrix();

        rlSetTexture(0);
    }

    // Draw cube with texture piece applied to all faces
    void DrawCubeTextureRec(
        Texture2D texture, Rectangle source, Vector3 position, float width, float height, float length, Color color) {
        float x = position.x;
        float y = position.y;
        float z = position.z;
        float texWidth = (float)texture.width;
        float texHeight = (float)texture.height;

        // Set desired texture to be enabled while drawing following vertex data
        rlSetTexture(texture.id);

        // We calculate the normalized texture coordinates for the desired texture-source-rectangle
        // It means converting from (tex.width, tex.height) coordinates to [0.0f, 1.0f] equivalent
        rlBegin(RL_QUADS);
        rlColor4ub(color.r, color.g, color.b, color.a);

        // Front face
        rlNormal3f(0.0f, 0.0f, 1.0f);
        rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
        rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
        rlTexCoord2f(source.x / texWidth, source.y / texHeight);
        rlVertex3f(x - width / 2, y + height / 2, z + length / 2);

        // Back face
        rlNormal3f(0.0f, 0.0f, -1.0f);
        rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
        rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
        rlTexCoord2f(source.x / texWidth, source.y / texHeight);
        rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
        rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x + width / 2, y - height / 2, z - length / 2);

        // Top face
        rlNormal3f(0.0f, 1.0f, 0.0f);
        rlTexCoord2f(source.x / texWidth, source.y / texHeight);
        rlVertex3f(x - width / 2, y + height / 2, z - length / 2);
        rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
        rlVertex3f(x + width / 2, y + height / 2, z - length / 2);

        // Bottom face
        rlNormal3f(0.0f, -1.0f, 0.0f);
        rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
        rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
        rlTexCoord2f(source.x / texWidth, source.y / texHeight);
        rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
        rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x + width / 2, y - height / 2, z + length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x - width / 2, y - height / 2, z + length / 2);

        // Right face
        rlNormal3f(1.0f, 0.0f, 0.0f);
        rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x + width / 2, y - height / 2, z - length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
        rlVertex3f(x + width / 2, y + height / 2, z - length / 2);
        rlTexCoord2f(source.x / texWidth, source.y / texHeight);
        rlVertex3f(x + width / 2, y + height / 2, z + length / 2);
        rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x + width / 2, y - height / 2, z + length / 2);

        // Left face
        rlNormal3f(-1.0f, 0.0f, 0.0f);
        rlTexCoord2f(source.x / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x - width / 2, y - height / 2, z - length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, (source.y + source.height) / texHeight);
        rlVertex3f(x - width / 2, y - height / 2, z + length / 2);
        rlTexCoord2f((source.x + source.width) / texWidth, source.y / texHeight);
        rlVertex3f(x - width / 2, y + height / 2, z + length / 2);
        rlTexCoord2f(source.x / texWidth, source.y / texHeight);
        rlVertex3f(x - width / 2, y + height / 2, z - length / 2);

        rlEnd();

        rlSetTexture(0);
    }

    Model MakeWallModel(Texture2D texture, float width, float height, float tileSize) {
        SetTextureWrap(texture, TEXTURE_WRAP_REPEAT);

        Mesh mesh = GenMeshPlane(width, height, 1, 1);

        float repeatU = width / tileSize;
        float repeatV = height / tileSize;

        for (int i = 0; i < mesh.vertexCount; i++) {
            mesh.texcoords[i * 2 + 0] *= repeatU;
            mesh.texcoords[i * 2 + 1] *= repeatV;
        }

        UploadMesh(&mesh, false);

        Model model = LoadModelFromMesh(mesh);
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

        return model;
    }

    Model MakeVerticalWallModel(Texture2D texture, float width, float height, float tileSize) {
        // Create a custom vertical mesh instead of using GenMeshPlane + rotation
        Mesh mesh = {0};

        // 4 vertices for a quad
        mesh.vertexCount = 4;
        mesh.triangleCount = 2;

        // Allocate memory for mesh data
        mesh.vertices = (float*)RL_MALLOC(mesh.vertexCount * 3 * sizeof(float));
        mesh.texcoords = (float*)RL_MALLOC(mesh.vertexCount * 2 * sizeof(float));
        mesh.normals = (float*)RL_MALLOC(mesh.vertexCount * 3 * sizeof(float));
        mesh.indices = (unsigned short*)RL_MALLOC(mesh.triangleCount * 3 * sizeof(unsigned short));

        // Define vertices for a vertical wall (in YZ plane, facing +X direction)
        // Bottom-left
        mesh.vertices[0] = 0.0f;
        mesh.vertices[1] = 0.0f;
        mesh.vertices[2] = -width / 2;
        // Bottom-right
        mesh.vertices[3] = 0.0f;
        mesh.vertices[4] = 0.0f;
        mesh.vertices[5] = width / 2;
        // Top-left
        mesh.vertices[6] = 0.0f;
        mesh.vertices[7] = height;
        mesh.vertices[8] = -width / 2;
        // Top-right
        mesh.vertices[9] = 0.0f;
        mesh.vertices[10] = height;
        mesh.vertices[11] = width / 2;

        // Calculate texture coordinate repeats
        float repeatU = width / tileSize;
        float repeatV = height / tileSize;

        // Define texture coordinates with repetition
        mesh.texcoords[0] = 0.0f;
        mesh.texcoords[1] = 0.0f; // Bottom-left
        mesh.texcoords[2] = repeatU;
        mesh.texcoords[3] = 0.0f; // Bottom-right
        mesh.texcoords[4] = 0.0f;
        mesh.texcoords[5] = repeatV; // Top-left
        mesh.texcoords[6] = repeatU;
        mesh.texcoords[7] = repeatV; // Top-right

        // Define normals (all pointing in +X direction for a wall facing +X)
        for (int i = 0; i < mesh.vertexCount; i++) {
            mesh.normals[i * 3 + 0] = 1.0f; // X
            mesh.normals[i * 3 + 1] = 0.0f; // Y
            mesh.normals[i * 3 + 2] = 0.0f; // Z
        }

        // Define indices for two triangles
        mesh.indices[0] = 0;
        mesh.indices[1] = 1;
        mesh.indices[2] = 2; // First triangle
        mesh.indices[3] = 1;
        mesh.indices[4] = 3;
        mesh.indices[5] = 2; // Second triangle

        // Upload mesh to GPU
        UploadMesh(&mesh, false);

        // Create model from mesh
        Model model = LoadModelFromMesh(mesh);

        // Assign texture and set wrapping
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
        SetTextureWrap(model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture, TEXTURE_WRAP_REPEAT);
        SetTextureFilter(model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture, TEXTURE_FILTER_BILINEAR);

        return model;
    }

} // namespace tex
