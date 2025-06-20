#version 330

in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

uniform vec3 lightDir;
uniform sampler2D texture0;

void main() {
    vec3 norm = normalize(fragNormal);
    float diff = max(dot(-lightDir, norm), 0.0);

    vec4 texColor = texture(texture0, fragTexCoord);
    finalColor = vec4(texColor.rgb * diff, texColor.a);
}

