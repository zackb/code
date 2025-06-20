#version 330

in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

uniform vec3 lightDir;

void main() {
    vec3 normal = normalize(fragNormal);
    float diffuse = max(dot(-lightDir, normal), 0.0);
    vec3 color = vec3(0.0, 1.0, 0.0); // LIME green
    finalColor = vec4(color * diffuse, 1.0);
}

