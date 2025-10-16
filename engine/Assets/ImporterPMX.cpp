//在CPU端将模型数据提取，返回一个模型指针
#include "ImporterPMX.h"
#include "../Render/Texture2D.h"
#include "../Render/Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "Model.h"
#include <unordered_map>

std::shared_ptr<Model> ImporterPMX::Load(const std::string& filePath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
                                                aiProcess_Triangulate |
                                                aiProcess_FlipUVs |
                                                aiProcess_GenSmoothNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "[ImporterPMX] Assimp: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    auto model = std::make_shared<Model>();
    model->meshes.reserve(scene->mNumMeshes);
    auto materials = std::vector<std::shared_ptr<Material>>();//收集所有mesh的材质
    materials.reserve(scene->mNumMaterials);
    //存储所有纹理的路径
    static std::unordered_map<std::string, std::shared_ptr<Texture2D>> textureCache;
    //加载所有材质纹理
    for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
    {
        auto material = std::make_shared<Material>();
        aiMaterial* mat = scene->mMaterials[m];
        aiString texPath;
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) &&
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
        {
            std::string modelFolder = filePath.substr(0, filePath.find_last_of("/\\") + 1);
            std::string fullPath = modelFolder + texPath.C_Str();
            //对是否已有纹理路径检查
            auto it = textureCache.find(fullPath);
            if (it != textureCache.end())
            {
                material->addTexture(it->second);
                std::cout << "[Cache] Reusing texture: " << fullPath << std::endl;
            }
            else
            {
                auto tex = std::make_shared<Texture2D>(fullPath);
                textureCache[fullPath] = tex;
                material->addTexture(tex);
                std::cout << "[Load] New texture: " << fullPath << std::endl;
            }
            materials.push_back(material);
            //std::cout << fullPath << std::endl;
        }
    }

    //转换网格
    unsigned int baseVertex = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* aMesh = scene->mMeshes[i];
        MeshCPU mesh;

        mesh.vertices.reserve(aMesh->mNumVertices);
        for (unsigned int v = 0; v < aMesh->mNumVertices; ++v)
        {
            Vertex vert{};
            vert.pos = { aMesh->mVertices[v].x, aMesh->mVertices[v].y, aMesh->mVertices[v].z };
            vert.normal = { aMesh->mNormals[v].x, aMesh->mNormals[v].y, aMesh->mNormals[v].z };
            //判断该mesh是否存在uv
            if (aMesh->mTextureCoords[0])
                vert.uv = { aMesh->mTextureCoords[0][v].x,aMesh->mTextureCoords[0][v].y };
            mesh.vertices.push_back(vert);
        }
        //顶点索引
        mesh.indices.reserve(aMesh->mNumFaces * 3);
        for (unsigned int f = 0; f < aMesh->mNumFaces; ++f)
        {
            aiFace face = aMesh->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx)
                mesh.indices.push_back(face.mIndices[idx]);
        }

        mesh.material = materials[aMesh->mMaterialIndex];
        model->meshes.push_back(std::move(mesh));
    }

    std::cout << "[ImporterPMX] loaded " << model->meshes.size() << " meshes\n";
    return model;
}
