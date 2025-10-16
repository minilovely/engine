#pragma once
#include <memory>
#include <vector>
#include "Component.h"
#include "../Render/MeshGPU.h"

struct MeshCPU;
class MeshGPU;
class Model;

class Mesh : public Component
{
public:
    Mesh(Actor* owner);
    void SetSingleMesh(const MeshCPU& cpu);
    MeshGPU* getMeshGPU() const { return mesh_gpu.get(); }
private:
    std::unique_ptr<MeshGPU> mesh_gpu;
};

