#include "Model.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "renderer/Material.h"
#include "Mesh.h"
#include "renderer/Texture.h"
#include "renderer/Vertex.h"
#include "core/ResourceManager.h"

struct Vertex;

Model::Model(const std::filesystem::path& path, ResourceManager* rm)
{
    std::cout << "Loading Model: " << path << std::endl;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeMeshes);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    std::filesystem::path modelDir = path.parent_path();

    processNode(scene->mRootNode, scene, path, rm);
}

void Model::processNode(const aiNode* node, const aiScene* scene, const std::filesystem::path& path, ResourceManager* rm)
{
    for (unsigned int i = 0; i< node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        subMeshes_.push_back(processMesh(mesh, scene, path, node->mMeshes[i], rm));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene, path, rm);
    }
}

Model::SubMesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const std::filesystem::path& path, unsigned int index, ResourceManager* rm)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Handle materialHandle{};

    // vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.Position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };
        vertex.Normal = {
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        };
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        } else
        {
            vertex.TexCoords = {0, 0};
        }
        vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // material
    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        materialHandle = processMaterial(material, path, mesh->mMaterialIndex, rm);
    } else
        materialHandle = rm->materialPool.GetHandle("default");

    MeshData meshData;
    meshData.indices = indices;
    meshData.vertices = vertices;

    Mesh gpuMesh = meshData.UploadToGPU();

    SubMesh subMesh{};
    subMesh.materialHandle = materialHandle;
    std::string meshKey = path.string() + "::mesh_" + std::to_string(index);
    subMesh.meshHandle = rm->meshPool.Register(meshKey, std::move(gpuMesh));

    return subMesh;
}

Handle Model::processMaterial(aiMaterial* mat, const std::filesystem::path& path, unsigned int index, ResourceManager* rm)
{
    std::string materialKey = path.string() + "::mat_" + std::to_string(index);

    if (Handle materialHandle = rm->materialPool.GetHandle(materialKey))
        return materialHandle;

    Material material;
    Handle shaderHandle = rm->shaderPool.GetHandle("texShadable");

    if (!shaderHandle)
    {
        std::cout << "Shader has not been compiled yet." << std::endl;
        Shader::ShaderContext sc = {
            BASE_PATH / "resources/shaders/texShadable.vert",
            {},
            {},
            {},
            BASE_PATH / "resources/shaders/texShadable.frag",
            {}
        };

        int error = 0;
        auto shader = Shader(sc, error);

        if (error)
        {
            std::cerr << "Error parsing material with shader: " << "texShadable" << std::endl;
            return {0, 0};
        }

        shaderHandle = rm->shaderPool.Register("texShadable", std::move(shader));
    }

    material.Init(shaderHandle, *rm);

    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString str;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        std::string texFileName = str.C_Str();

        std::filesystem::path fullTexPath = DetermineTexturePath(path, texFileName);

        std::string texKey = fullTexPath.string();
        Handle texHandle = rm->texturePool.GetHandle(texKey);

        if (!texHandle)
            texHandle = rm->texturePool.Register(texKey, Texture(fullTexPath.string()));

        material.SetTexture("material.diffuse", texHandle);
    }

    aiString str;
    bool specularFound = false;
    if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        mat->GetTexture(aiTextureType_SPECULAR, 0, &str);
        specularFound = true;
    } else if (mat->GetTextureCount(aiTextureType_SHININESS) > 0)
    {
        mat->GetTexture(aiTextureType_SHININESS, 0, &str);
        specularFound = true;
    }

    if (specularFound)
    {
        std::string texFileName = str.C_Str();

        std::filesystem::path fullTexPath = DetermineTexturePath(path, texFileName);

        std::string texKey = fullTexPath.string();
        Handle texHandle = rm->texturePool.GetHandle(texKey);

        if (!texHandle)
            texHandle = rm->texturePool.Register(texKey, Texture(fullTexPath.string()));

        material.SetTexture("material.specular", texHandle);
    }

#ifdef DEBUG_BUILD
    aiColor4D diffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
    aiColor4D ambientColor(0.0f, 1.0f, 0.0f, 1.0f);
    aiColor4D specularColor(0.0f, 0.0f, 1.0f, 1.0f);
#else
    aiColor4D diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    aiColor4D ambientColor(0.0f, 0.0f, 0.0f, 1.0f);
    aiColor4D specularColor(1.0f, 1.0f, 1.0f, 1.0f);
#endif


    float shininess = 32.0f;
    aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);
    material.SetValue("material.shininess", shininess);

    aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
    material.SetValue("material.diffuseColor", glm::vec3{diffuseColor[0], diffuseColor[1], diffuseColor[2]});

    aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambientColor);
    material.SetValue("material.ambientColor", glm::vec3{ambientColor[0], ambientColor[1], ambientColor[2]});

    aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specularColor);
    material.SetValue("material.specularColor", glm::vec3{specularColor[0], specularColor[1], specularColor[2]});


    return rm->materialPool.Register(materialKey, material);
}

std::filesystem::path Model::DetermineTexturePath(const std::filesystem::path& modelPath, const std::string& textureFilename)
{
    const std::filesystem::path modelDir = modelPath.parent_path();

    const std::filesystem::path texRelativePath(textureFilename);

    std::filesystem::path fullTexturePath;
    if (texRelativePath.is_relative())
        fullTexturePath = modelDir / texRelativePath;
    else
        fullTexturePath = modelDir / texRelativePath.filename();

    if (!std::filesystem::exists(fullTexturePath))
    {
        std::cerr << "[Error] Texture not found: " << fullTexturePath << "\n";
    }

    return fullTexturePath;
}