#include "MeshGPU.h"
#include "glad.h"

#include<iostream>
MeshGPU::MeshGPU(const std::vector<Vertex>& verts,
                    const std::vector<unsigned int>& index,
                    const std::shared_ptr<Material>& material)
	: verts(verts), index(index), material(material)
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
//°ó¶¨VAO£¬²ÄÖÊ
void MeshGPU::Bind() const
{
    glBindVertexArray(vao);
    if (material)
    {
        material->bindTextures();
    }
    else
    {
        std::cout << "Material lose!" << std::endl;
    }
}

void MeshGPU::Draw() const
{
    glPointSize(10.0f);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, nullptr);
}
