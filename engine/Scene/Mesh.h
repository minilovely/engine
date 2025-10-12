#pragma once
#include <memory>
#include <vector>
#include "Component.h"
#include "../Render/MeshGPU.h"

struct MeshCPU;
class MeshGPU;
class Model;

// 修复：确保Component已正确定义并包含头文件
class Mesh : public Component
{
public:
    Mesh(Actor* owner);
    //将CPU数据转到GPU
    void SetModel(std::shared_ptr<Model> m);
    void SetSingleMesh(const MeshCPU& cpu);
    void Render() const override;

    unsigned int getVAO() { return gpuMeshes.empty() ? 0 : gpuMeshes.front()->getVAO(); };
    unsigned int getIndexCount() { return gpuMeshes.empty() ? 0 : gpuMeshes.front()->getIndexCount(); }
private:
    std::shared_ptr<Model> model;
    std::vector<std::unique_ptr<MeshGPU>> gpuMeshes;
};

