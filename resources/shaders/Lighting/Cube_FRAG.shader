#shader fragment
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 u_ObjColor;
uniform bool u_Debug;
uniform vec3 u_LightColor;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform float u_specularStrength;
uniform int u_pow;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_pow);
    vec3 specular = u_specularStrength * spec * u_LightColor;
    if (u_Debug)
    {
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    } else
    {
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * u_LightColor;
        FragColor = vec4((ambient + diffuse + specular) * u_ObjColor, 1.0f);
    }
}