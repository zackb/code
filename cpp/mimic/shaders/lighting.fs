#version 330

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2D texture0;

uniform vec3 lightDir;    // direction TO light
uniform vec3 ambientColor;
uniform vec3 lightColor;

out vec4 finalColor;

void main() {
    // Normalize normal and light
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(lightDir);

    // Basic diffuse lighting
    float diff = max(dot(N, L), 0.0);

    // Texture color
    vec4 texColor = texture(texture0, fragTexCoord);

    // Combine ambient and diffuse
    vec3 result = ambientColor * texColor.rgb + lightColor * diff * texColor.rgb;

    // finalColor = vec4(normalize(fragNormal) * 0.5 + 0.5, 1.0);
    finalColor = vec4(result, texColor.a);
}

