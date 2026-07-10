#pragma once
#include <filesystem>
#include <vector>

#include "../AssetPool.h"


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
    Model() = default;
    explicit Model(const std::filesystem::path& path);

    [[nodiscard]] const std::vector<SubMesh>& GetSubMeshes() const { return subMeshes_; }


private:
    void processNode(aiNode *node, const aiScene *scene, const std::filesystem::path& path);
    static SubMesh processMesh(aiMesh *mesh, const aiScene * scene, const std::filesystem::path& path, unsigned int index);
    static Handle processMaterial(aiMaterial* mat, const std::filesystem::path& path, unsigned int index);
    static std::filesystem::path DetermineTexturePath(const std::filesystem::path& modelPath, const std::string& textureFilename);

private:
    std::vector<SubMesh> subMeshes_;

};
