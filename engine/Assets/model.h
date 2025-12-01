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
struct FaceMorph
{
	std::string name;
	std::vector<glm::vec3> positionOffsets;
	std::vector<glm::vec3> normalOffsets;
	float weight = 0.0f;
};
struct MeshCPU
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;

	std::vector<int> bones;//记录该mesh用到的骨骼索引
	std::vector<FaceMorph> faceMorphs;
	std::unordered_map<std::string, int> faceMorphMap;
};

//Model类存放顶点CPU端数据，控制ImporterPMX的输出为完整模型
class Model 
{
public:
    std::vector<MeshCPU> meshes;
	Skeleton skeleton;//骨架
};
