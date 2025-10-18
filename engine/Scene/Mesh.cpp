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
    mesh_gpu =  std::make_unique<MeshGPU>(cpu.vertices, cpu.indices, cpu.material);
}



