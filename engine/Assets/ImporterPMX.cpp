#include "ImporterPMX.h"
#include "Model.h"
#include "../Render/Texture2D.h"
#include "../Render/Material.h"
#include "../Assets/MaterialAssets.h"
#include "Bone.h"

#include <iostream>
#include <unordered_map>

static glm::mat4 AssimpToGlm(const aiMatrix4x4& m)/*assimp是行优先，glm存储与OpenGL一致，为列优先*/
{
    return glm::transpose(glm::make_mat4(&m.a1));
}

std::shared_ptr<Model> ImporterPMX::Load(const std::string& filePath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
                                                aiProcess_Triangulate | aiProcess_FlipUVs |
                                                aiProcess_ConvertToLeftHanded /*这是左手*/ |
                                                aiProcess_GenSmoothNormals );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "[ImporterPMX] Assimp: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    auto model = std::make_shared<Model>();
    model->meshes.reserve(scene->mNumMeshes);
    auto materials = std::vector<std::shared_ptr<Material>>();//收集所有mesh的材质
    materials.reserve(scene->mNumMaterials);
    
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
            auto tex = MaterialAssets::getTexture(fullPath);
            if (tex)
            {
                material->addTexture(tex);
            }
            materials.push_back(material);
            //std::cout << fullPath << std::endl;
        }
    }
    //转换网格
    unsigned int baseVertex = 0;
    int rootBoneIndex = 0;
    //构建全局骨骼表
    Skeleton& skeleton = model->skeleton;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* aMesh = scene->mMeshes[i];
        MeshCPU mesh;

        std::vector<TempVertex> tempVertices;
        tempVertices.resize(aMesh->mNumVertices);
        mesh.vertices.reserve(aMesh->mNumVertices);

        for (unsigned int v = 0; v < aMesh->mNumVertices; ++v)
        {
            tempVertices[v].pos = { aMesh->mVertices[v].x, aMesh->mVertices[v].y, aMesh->mVertices[v].z };
            tempVertices[v].normal = { aMesh->mNormals[v].x, aMesh->mNormals[v].y, aMesh->mNormals[v].z };
            //判断该mesh是否存在uv
            if (aMesh->mTextureCoords[0])
            {
                tempVertices[v].uv = { aMesh->mTextureCoords[0][v].x,aMesh->mTextureCoords[0][v].y };
            }
        }
        //顶点索引
        mesh.indices.reserve(aMesh->mNumFaces * 3);
        for (unsigned int f = 0; f < aMesh->mNumFaces; ++f)
        {
            aiFace face = aMesh->mFaces[f];
            for (unsigned int idx = 0; idx < face.mNumIndices; ++idx)
                mesh.indices.push_back(face.mIndices[idx]);
        }
		//收集骨骼到全局骨骼表
        for (unsigned int j = 0; j < aMesh->mNumBones; j++)
        {
            aiBone* aBone = aMesh->mBones[j];
            std::string boneName = aBone->mName.C_Str();
            //存储骨骼偏移矩阵，为每个骨骼创建索引
			skeleton.addIfNew(boneName, AssimpToGlm(aBone->mOffsetMatrix));
            int boneIndex = skeleton.boneMap[boneName];//这里返回的就是skeleton建立的索引
            for (unsigned int k = 0; k < aBone->mNumWeights; k++)
            {
				aiVertexWeight weight = aBone->mWeights[k];
				unsigned int vertexId = weight.mVertexId;
				float weightValue = weight.mWeight;
                tempVertices[vertexId].boneInfluences.push_back({ boneIndex, weightValue });
            }
        }
        for (unsigned int v = 0; v < tempVertices.size(); v++)
        {
            auto& temp = tempVertices[v];
            Vertex finalVertex;
            finalVertex.pos = temp.pos;
            finalVertex.normal = temp.normal;
            finalVertex.uv = temp.uv;

            // 排序和裁剪
            auto& influences = temp.boneInfluences;

            std::sort(influences.begin(), influences.end(),
                [](const std::pair<int, float>& a, const std::pair<int, float>& b)
                {
                    return a.second > b.second;  // 权重大的在前
                });
            int numBones = std::min(4, (int)influences.size());
            float weightSum = 0.0f;
            for (int i = 0; i < numBones; ++i)
            {
                finalVertex.boneIDs[i] = influences[i].first;   // boneIndex
                finalVertex.weights[i] = influences[i].second;  // weight
                weightSum += influences[i].second;
            }
            // 归一化权重（确保总和为1.0）
            if (weightSum > 0.0f)
            {
                for (int i = 0; i < numBones; ++i)
                {
                    finalVertex.weights[i] /= weightSum;
                }
            }
            // 清空剩余槽位
            for (int i = numBones; i < 4; ++i)
            {
                finalVertex.boneIDs[i] = -1;
                finalVertex.weights[i] = 0.0f;
            }
            mesh.vertices.push_back(finalVertex);
        }
		//收集该mesh用到的骨骼索引
		std::unordered_set<int> meshBoneSet;
        for(auto& v : mesh.vertices)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (v.boneIDs[i] != -1)
                {
                    meshBoneSet.insert(v.boneIDs[i]);
                }
            }
		}
		mesh.bones.assign(meshBoneSet.begin(), meshBoneSet.end());

        mesh.material = materials[aMesh->mMaterialIndex];
        model->meshes.push_back(std::move(mesh));
    }
	BuildBoneHierarchy(scene->mRootNode, model->skeleton, -1);
    //调试验证
    //std::cout << "=== 骨骼层级验证 ===" << std::endl;
    //for (size_t i = 0; i < skeleton.bones.size(); ++i)
    //{
    //    std::cout << "Bone[" << i << "]: " << skeleton.bones[i].name
    //        << " -> Parent: " << skeleton.bones[i].parent << std::endl;
    //}

    return model;
}

void ImporterPMX::BuildBoneHierarchy(const aiNode* node, Skeleton& skeleton, int parentIndex)
{
	std::string nodeName = node->mName.C_Str();
    int currentBoneIndex = parentIndex;
	auto it = skeleton.boneMap.find(nodeName);
    if (it != skeleton.boneMap.end())
    {
        currentBoneIndex = it->second;
        skeleton.bones[currentBoneIndex].parent = parentIndex;

        skeleton.bones[currentBoneIndex].localTransMat = AssimpToGlm(node->mTransformation);
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
			BuildBoneHierarchy(node->mChildren[i], skeleton, currentBoneIndex);
        }
	}
    else
    {
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            BuildBoneHierarchy(node->mChildren[i], skeleton, parentIndex);
        }
    }
}
