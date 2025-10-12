#pragma once
#include <vector>
#include "../Core/math.h"
#include "../Assets/model.h"

class MeshGPU
{
public:
    //CPU数据转入GPU，将传入的顶点及其索引装入vao,ebo,vbo
    MeshGPU(const std::vector<Vertex>& verts, 
            const std::vector<unsigned int>& index,
            unsigned int diffuseTex);
    ~MeshGPU();

    void Bind() const;
    void Draw() const;

    unsigned int getVAO()        const { return vao; }
    unsigned int getIndexCount() const { return indexCount; }

private:
    unsigned int vao = 0, vbo = 0, ebo = 0;
    unsigned indexCount = 0;
    unsigned int diffuseTex = 0;
};

