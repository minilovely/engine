#pragma once
#include "../Render/Material.h"
#include "Bone.h"
#include <vector>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;

    int boneIDs[4] = { -1, -1, -1, -1 };  // -1表示无影响
    float weights[4] = { 0, 0, 0, 0 };
};

struct MeshCPU
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;

	std::vector<int> bones;//记录该mesh用到的骨骼索引
};
//Model类存放顶点CPU端数据，控制ImporterPMX的输出为完整模型
class Model 
{
public:
    std::vector<MeshCPU> meshes;
	Skeleton skeleton;//骨架
};