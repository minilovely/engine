#pragma once
#include <memory>
#include <vector>
#include "Component.h"
#include "../Render/MeshGPU.h"
#include "../RenderPiPeline/Pass.h"

struct MeshCPU;
class MeshGPU;
class Model;

class Mesh : public Component
{
public:
    Mesh(Actor* owner);
    void transCPUToGPU(const MeshCPU& cpu);
    
    MeshGPU* getMeshGPU() const { return mesh_gpu.get(); }
    int getValue() const { return value; }
    void setValue(int v) { value = v; }

    void addPass(std::shared_ptr<Pass> p) { passes.push_back(std::move(p)); }
    const auto& getPasses() const { return passes; }
    void setDepthWrite(bool value) { depthWrite = value; }
    bool getDepthWrite() const { return depthWrite; }
    void setColorWrite(bool value) { colorWrite = value; }
    bool getColorWrite() { return colorWrite; }
private:
    std::unique_ptr<MeshGPU> mesh_gpu;
    std::vector<std::shared_ptr<Pass>> passes;
    int value = 2000;
    bool depthWrite = true;
    bool colorWrite = true;
};

