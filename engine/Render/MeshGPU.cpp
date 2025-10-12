#include "MeshGPU.h"
#include "glad.h"
#include<iostream>
MeshGPU::MeshGPU(const std::vector<Vertex>& verts,
                    const std::vector<unsigned int>& index,
                    unsigned int diffuseTex)
	: indexCount(static_cast<unsigned int>(index.size())), diffuseTex(diffuseTex)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(uint32_t), index.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                            sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glBindVertexArray(0);
}

MeshGPU::~MeshGPU()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void MeshGPU::Bind() const
{
    glBindVertexArray(vao);
}

void MeshGPU::Draw() const
{
    //std::cout << "[MeshGPU] Drawing mesh with diffuseTex = " << diffuseTex << "\n";
    if (diffuseTex != 0) 
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
    }
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}
