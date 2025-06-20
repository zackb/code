#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform float time;

void main() {
    float t = fragTexCoord.y;
    vec3 top = vec3(0.1, 0.2, 0.5);     // Sky top color
    vec3 bottom = vec3(0.9, 0.9, 1.0);  // Sky bottom color
    vec3 color = mix(bottom, top, t);
    finalColor = vec4(color, 1.0);
}

