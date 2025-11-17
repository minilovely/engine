#pragma once
#include "../Core/math.h"
#include "../Render/MeshGPU.h"
#include "../Render/Material.h"
#include "../Render/ShadowMap.h"
#include "../Assets/PassAssets.h"

struct RenderCommand
{
    MeshGPU*                  mesh = nullptr;
    Material*                 material;
    int                       value = 2000;

    glm::mat4                 M;
    glm::mat4                 lightSpaceMatrix;
    glm::mat4                 MVP;
    glm::vec3                 viewPos;
    int                       lightCount = 0;
    
    bool                      depthWrite = true;
    bool                      colorWrite = true;
    std::string               cullMode = "Back";
    std::shared_ptr<ShadowMap> shadowMap = nullptr;
    std::shared_ptr<PassAssets> shadowAssets = nullptr;

	std::vector<glm::mat4>   uBoneMats;

    bool hasBones = false;

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
    void DrawShadow();
    void DrawForward();
    const std::vector<RenderCommand>& getRenderCommands() const { return cmds; }
    void SetShadowMap(std::shared_ptr<ShadowMap> sm) { shadowMap = std::move(sm); }
private:
    std::vector<RenderCommand> cmds;
    std::shared_ptr<ShadowMap> shadowMap;
};

