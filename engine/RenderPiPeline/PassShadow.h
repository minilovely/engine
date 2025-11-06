#pragma once
#include "Pass.h"
#include "../Render/ShadowMap.h"
#include "../System/LightManager.h"
#include "../Assets/PassAssets.h"

#include <memory>
#include <glm/glm.hpp>

class PassShadow : public Pass
{
public:
    PassShadow();
    void Init() override;
    void Collect(const Camera& camera, Mesh* mesh, RenderQueue& outQueue) override;
    std::shared_ptr<ShadowMap> GetShadowMap() const { return shadowMap; }
    glm::mat4 GetLightSpaceMatrix() const { return lightSpaceMatrix; }
    std::shared_ptr<PassAssets> GetShadowAssets() const { return shadowAssets; }
private:
    LightManager* lightManager = nullptr;
    std::shared_ptr<ShadowMap> shadowMap;
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
    std::shared_ptr<PassAssets> shadowAssets;
};

