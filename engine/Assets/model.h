#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct MeshCPU {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int diffuseTex = 0;
};
//Model类存放顶点CPU端数据，控制ImporterPMX的输出为完整模型
class Model 
{
public:
    std::vector<MeshCPU> meshes;
};