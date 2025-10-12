#include "Mesh.h"
#include "../Render/MeshGPU.h"
#include "../Assets/model.h"
#include <glad.h>
#include<iostream>
Mesh::Mesh(Actor* owner) : Component(owner)
{
}

void Mesh::SetModel(std::shared_ptr<Model> m)
{
    model = std::move(m);
    if (!model)
    {
        std::cout << "MeshComponent: No Model exists !" << std::endl;
        return;
    }
    else
    {
        std::cout << "[Mesh] Loaded " << model->meshes.size() << " meshes\n";
    }
    gpuMeshes.clear();
    gpuMeshes.reserve(model->meshes.size());
    for (const MeshCPU& cpu : model->meshes)
        gpuMeshes.emplace_back(std::make_unique<MeshGPU>(cpu.vertices,
                                                            cpu.indices,
                                                            cpu.diffuseTex));
    //µ÷ÊÔmeshÊý¾Ý
    for (size_t i = 0; i < model->meshes.size(); ++i) 
    {
        std::cout << "  - Mesh " << i << " has " << model->meshes[i].indices.size() / 3
            << " triangles, diffuseTex = " << model->meshes[i].diffuseTex << "\n";
    }
}

void Mesh::SetSingleMesh(const MeshCPU& cpu)
{
    gpuMeshes.clear();
    gpuMeshes.emplace_back(std::make_unique<MeshGPU>(
        cpu.vertices, cpu.indices, cpu.diffuseTex));
}

void Mesh::Render() const
{
    for (const auto& gpu : gpuMeshes)
    {
        gpu->Bind();
        gpu->Draw();
    }
}


