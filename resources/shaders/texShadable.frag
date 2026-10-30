#version 330 core
layout(location = 0) out vec4 FragColor;

#define MAX_LIGHTS 3

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 uViewPos;
uniform int uPointLightCount;

struct Material
{
    vec3 diffuseColor;
    sampler2D diffuse;
    vec3 specularColor;
    sampler2D specular;
    vec3 ambientColor;
    float shininess;
};
uniform Material material;

struct Light {
    int type; // 0 = dir, 1 = point, 2 = spot

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};
uniform Light lights[MAX_LIGHTS];

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < uPointLightCount; i++)
    {
        if (lights[i].type == 0)
            result += CalcDirLight(lights[i], norm, viewDir);
        else if (lights[i].type == 1)
            result += CalcPointLight(lights[i], norm, FragPos, viewDir);
        else
            result += vec3(0.1f, 0.1f, 0.1f);
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    float spec = 0.0;
    if (diff > 0.0)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords)) * material.ambientColor;
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords)) * material.diffuseColor;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)) * material.specularColor;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    float spec = 0.0;
    if (diff > 0.0)
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords)) * material.ambientColor;
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords)) * material.diffuseColor;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)) * material.specularColor;

    return (ambient + diffuse + specular);
}