#include "PassForward.h"
#include "../Scene/Camera.h"
#include "../Scene/Mesh.h"
#include "../Scene/Transform.h"
#include "../Scene/Light.h"
#include "../Scene/Actor.h"
#include "../System/LightManager.h"
#include "../RenderPiPeline/RenderQueue.h"
#include <iostream>

void PassForward::Init()
{
    std::string json_path = "Assets/Passes_json/model.json";
    asset = PassAssets::Load(json_path);
    auto vs_src = PassAssets::ReadText(asset->getVSPath());
    auto fs_src = PassAssets::ReadText(asset->getFSPath());
    shader = std::make_shared<Shader>(vs_src, fs_src);

}

void PassForward::Collect(const Camera& camera, const std::vector<Mesh*>& meshes, RenderQueue& outQueue)
{
    const glm::mat4 V = camera.getViewMatrix();
    const glm::mat4 P = camera.getProjectionMatrix();
    const glm::vec3 camPos = camera.getPosition();

    auto& lightManager = LightManager::Get();
    lightManager.upLoadToShader(shader.get());
    shader->setInt("lightCount", lightManager.getLightCount());

    for (Mesh* mc : meshes)
    {
        auto* owner = mc->GetOwner();
        auto* trans = owner->GetComponent<Transform>();
        auto* gpuMesh = mc->getMeshGPU();
        auto  mat = gpuMesh->getMaterial();

        RenderCommand cmd;
        cmd.mesh = gpuMesh;
        cmd.modelMatrix = trans->getModelMatrix();
        cmd.MVP = P * V * cmd.modelMatrix;
        cmd.M = trans->getModelMatrix();
        cmd.viewPos = camPos;
        cmd.material = mat;
        cmd.shader = shader;
        cmd.value = mc->getValue();
        cmd.lightCount = lightManager.getLightCount();
        cmd.material->setShader(shader);
        outQueue.Add(cmd);
    }
}

void PassForward::ReloadShader(const std::string& vsSrc, const std::string& fsSrc)
{
    shader = std::make_unique<Shader>(vsSrc, fsSrc);
}
