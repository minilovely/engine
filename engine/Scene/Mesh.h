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
    void addPass(std::shared_ptr<Pass> p) { passes.push_back(std::move(p)); }

    int getValue() const { return value; }
    void setValue(int v) { value = v; }
    const auto& getPasses() const { return passes; }
    void setDepthWrite(bool value) { depthWrite = value; }
    bool getDepthWrite() const { return depthWrite; }
    void setColorWrite(bool value) { colorWrite = value; }
    bool getColorWrite() { return colorWrite; }
    void setCullMode(std::string mode) { cullMode = mode; }
    std::string getCullMode() const { return cullMode; }
private:
    std::unique_ptr<MeshGPU> mesh_gpu;
    std::vector<std::shared_ptr<Pass>> passes;
    int value = 2000;
    bool depthWrite = true;
    bool colorWrite = true;
    std::string cullMode = "Back";
};

