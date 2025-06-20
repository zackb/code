#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 matModel;

// Output to fragment shader
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;

void main()
{
    // world position
    fragPosition = (matModel * vec4(vertexPosition, 1.0)).xyz;
    
    // texture coordinates
    fragTexCoord = vertexTexCoord;
    
    // transformed to world space
    fragNormal = normalize((matModel * vec4(vertexNormal, 0.0)).xyz);
    
    // final vertex position
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
