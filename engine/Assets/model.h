#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../Render/Material.h"
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct MeshCPU
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::shared_ptr<Material> material;
};
//Model类存放顶点CPU端数据，控制ImporterPMX的输出为完整模型
class Model 
{
public:
    std::vector<MeshCPU> meshes;
};