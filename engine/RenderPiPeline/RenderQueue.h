#pragma once
#include "../Core/math.h"
#include "../Render/MeshGPU.h"
#include "../Render/Material.h"
#include "../Render/ShadowMap.h"
#include "../Assets/PassAssets.h"

struct RenderCommand
{
    MeshGPU*  mesh;
    Material*                 material;
    int                       value = 2000;

    glm::mat4                 MVP;
    glm::mat4                 M;
    glm::vec3                 viewPos;
    int                       lightCount = 0;

    bool                      depthWrite = true;
    bool                      colorWrite = true;
    std::string               cullMode = "Back";
    std::shared_ptr<ShadowMap> shadowMap;
    std::shared_ptr<PassAssets> shadowAssets;

    enum class PassType
    {
        Forward, Shadow
    }PassType = PassType::Forward;
};

//struct ShadowCommand
//{
//    MeshGPU* mesh;
//    glm::mat4                 lightMat4 = glm::mat4(1.0f);
//    int                       lightIndex = 0;
//};

class RenderQueue
{
public:
    RenderQueue() = default;
    ~RenderQueue() = default;
    void Clear();
    void Add(const RenderCommand& cmd);
    void Sort();
    void DrawShadow();
    void DrawForward();
    const std::vector<RenderCommand>& getRenderCommands() const { return cmds; }

private:
    std::vector<RenderCommand> cmds;
    std::shared_ptr<ShadowMap> shadowMap;
};

