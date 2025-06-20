#version 330

// Input from vertex shader
in vec3 fragPosition;    // World position
in vec2 fragTexCoord;    // UV coordinates
in vec3 fragNormal;

out vec4 finalColor;
uniform float time;

void main() {
    float radius = 100.0; // match skydome!
    float t = (fragPosition.y + radius) / (2.0 * radius);
    t = clamp(t, 0.0, 1.0);
    
    float smoothT = smoothstep(0.0, 1.0, t);
    
    vec3 top = vec3(0.1, 0.2, 0.5);
    vec3 bottom = vec3(0.9, 0.9, 1.0);
    vec3 color = mix(bottom, top, smoothT);
    
    finalColor = vec4(color, 1.0);
}
