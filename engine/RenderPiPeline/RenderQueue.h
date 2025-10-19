#pragma once
#include "../Core/math.h"
#include "../Render/MeshGPU.h"
#include "../Render/Material.h"

struct RenderCommand
{
    MeshGPU*  mesh;
    Material*                 material;
    int                       value = 2000;         //渲染队列的值

    glm::mat4                 MVP;
    glm::mat4                 M;
    glm::vec3                 viewPos;
    int                       lightCount = 0;       // 可扩展任意 uniform

    enum class PassType
    {
        Forward, Shadow
    }PassType = PassType::Forward;
};

class RenderQueue
{
public:
    RenderQueue() = default;
    ~RenderQueue() = default;
    void Clear();
    void Add(const RenderCommand& cmd);
    void Sort();
    void Draw() const;
    const std::vector<RenderCommand>& getRenderCommands() const { return cmds; }

private:
    std::vector<RenderCommand> cmds;
};

