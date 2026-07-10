#pragma once
#include <filesystem>
#include <vector>

#include "../AssetPool.h"


class ResourceManager;
struct Material;
struct MeshData;
class aiNode;
class aiScene;
class aiMesh;
class aiMaterial;

class Model
{
    struct SubMesh
    {
        Handle meshHandle;
        Handle materialHandle;
    };

public:
    explicit Model(const std::filesystem::path& path, ResourceManager* rm);

    [[nodiscard]] std::vector<SubMesh>& GetSubMeshes() { return subMeshes_; }


    Model(Model&& other) noexcept : subMeshes_(std::move(other.subMeshes_)) { }
    Model& operator=(Model&& other) noexcept
    {
        if (this != &other)
        {
            subMeshes_ = std::move(other.subMeshes_);

            other.subMeshes_.clear();
        }
        return *this;
    }

    Model(const Model& other) = delete;
    Model& operator=(const Model& other) = delete;


private:
    void processNode(aiNode *node, const aiScene *scene, const std::filesystem::path& path, ResourceManager* rm);
    static SubMesh processMesh(aiMesh *mesh, const aiScene * scene, const std::filesystem::path& path, unsigned int index, ResourceManager* rm);
    static Handle processMaterial(aiMaterial* mat, const std::filesystem::path& path, unsigned int index, ResourceManager* rm);
    static std::filesystem::path DetermineTexturePath(const std::filesystem::path& modelPath, const std::string& textureFilename);

private:
    std::vector<SubMesh> subMeshes_;

};
