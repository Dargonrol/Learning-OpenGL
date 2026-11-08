#version 330 core

layout (std140) uniform CameraData
{
    mat4 uView;
    mat4 uProj;
    vec4 uViewPos;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 uModel;

void main()
{
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
    FragPos = vec3(uModel* vec4(aPos, 1.0));
    Normal = aNormal;
    TexCoords = aTexCoords;
}