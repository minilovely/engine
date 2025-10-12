//在CPU端将模型数据提取，返回一个模型指针
#include "ImporterPMX.h"
#include "../Render/Texture2D.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <filesystem>
#include "Model.h"

static std::vector<std::shared_ptr<Texture2D>> textureCache;

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

    /* 1. 预加载所有材质纹理 */
    std::vector<unsigned int> materialTex(scene->mNumMaterials, 0);
    for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
    {
        aiMaterial* mat = scene->mMaterials[m];
        aiString texPath;
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) &&
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            std::string modelFolder = filePath.substr(0, filePath.find_last_of("/\\") + 1);
            std::string fullPath = modelFolder + texPath.C_Str();
            //完成meterial转化为OpenGL格式
            auto tex = std::make_shared<Texture2D>(fullPath);
            textureCache.push_back(tex);
            materialTex[m] = tex->getHandle();

            std::cout << fullPath<< std::endl;
        }
    }

    /* 2. 转换网格 */
    unsigned int baseVertex = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* aMesh = scene->mMeshes[i];
        MeshCPU mesh;

        mesh.vertices.reserve(aMesh->mNumVertices);
        for (unsigned int v = 0; v < aMesh->mNumVertices; ++v) {
            Vertex vert{};
            vert.pos = { aMesh->mVertices[v].x, aMesh->mVertices[v].y, aMesh->mVertices[v].z };
            vert.normal = { aMesh->mNormals[v].x, aMesh->mNormals[v].y, aMesh->mNormals[v].z };
            //判断该mesh是否存在uv
            if (aMesh->mTextureCoords[0])
                vert.uv = { aMesh->mTextureCoords[0][v].x,aMesh->mTextureCoords[0][v].y };
            mesh.vertices.push_back(vert);
        }

        mesh.indices.reserve(aMesh->mNumFaces * 3);
        for (unsigned int f = 0; f < aMesh->mNumFaces; ++f) {
            aiFace face = aMesh->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx)
                mesh.indices.push_back(face.mIndices[idx] /*+ baseVertex*/);
        }
        //baseVertex += aMesh->mNumVertices;

        mesh.diffuseTex = materialTex[aMesh->mMaterialIndex];
        model->meshes.push_back(std::move(mesh));
    }

    std::cout << "[ImporterPMX] loaded " << model->meshes.size() << " meshes\n";
    return model;
}
