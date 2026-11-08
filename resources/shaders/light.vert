#version 330 core

layout (std140) uniform CameraData
{
    mat4 uView;
    mat4 uProj;
    vec4 uViewPos;
};

layout (location = 0) in vec3 aPos;

uniform mat4 uModel;

void main()
{
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}