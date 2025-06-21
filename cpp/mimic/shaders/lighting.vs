#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 mvp;         // model-view-projection
uniform mat4 matModel;    // model matrix (for world-space normal)
uniform mat4 matNormal;   // inverse-transpose of model matrix
uniform vec3 lightDir;    // direction TO the light (normalized)
uniform vec3 viewPos;     // camera position (optional, for specular)

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCoord;

void main() {
    vec4 worldPos = matModel * vec4(vertexPosition, 1.0);
    fragPosition = worldPos.xyz;
    fragNormal = normalize((matNormal * vec4(vertexNormal, 0.0)).xyz);
    fragTexCoord = vertexTexCoord;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}

