#pragma once
#include "../Core/math.h"
#include "../Assets/model.h"
#include "Texture2D.h"
#include "Material.h"

#include <vector>

class MeshGPU
{
public:
    std::vector<int> localBonesIdx;

    //CPU数据转入GPU，将传入的顶点及其索引装入vao,ebo,vbo
    MeshGPU(const std::vector<Vertex>& verts, 
            const std::vector<unsigned int>& index,
            const std::shared_ptr<Material>& material);
    ~MeshGPU();

    void Bind() const;
    void Draw() const;

    Material* getMaterial() { return material.get(); }

private:
    unsigned int vao = 0, vbo = 0, ebo = 0;

    std::vector<Vertex> verts;
    std::vector<unsigned int> index;
    std::shared_ptr<Material> material;

};

