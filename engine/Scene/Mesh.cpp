#include "Mesh.h"
#include "../Render/MeshGPU.h"
#include "../Assets/model.h"

#include <glad.h>
#include<iostream>
Mesh::Mesh(Actor* owner) : Component(owner)
{

}

void Mesh::transCPUToGPU(const MeshCPU& cpu)
{
	std::unordered_map<int, int> boneMap;//全局骨骼索引到本地骨骼索引的映射
	for (size_t i = 0; i < cpu.bones.size(); ++i)
	{
		int globalBoneIdx = cpu.bones[i];
		boneMap[globalBoneIdx] = i;
	}
	std::vector<Vertex> localVertices = cpu.vertices;
	//修订顶点中的骨骼索引为本地索引
	for(auto& v : localVertices)
	{
		for (int i = 0; i < 4; ++i)
		{
			int globalBoneIdx = v.boneIDs[i];
			if (globalBoneIdx >= 0)
			{
				auto it = boneMap.find(globalBoneIdx);
				if (it != boneMap.end())
				{
					v.boneIDs[i] = it->second;
				}
				else
				{
					// 不应该发生，说明顶点权重有错误
					v.boneIDs[i] = 0;
					std::cerr << "[Mesh] Warning: Bone index " << globalBoneIdx
						<< " not found in usedGlobalBoneIndices" << std::endl;
				}
			}
		}
	}
	mesh_gpu = std::make_unique<MeshGPU>(localVertices, cpu.indices, cpu.material);
	mesh_gpu->localBonesIdx = cpu.bones;

}



