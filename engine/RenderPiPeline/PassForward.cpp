#include "PassForward.h"
#include "../Scene/Camera.h"
#include "../Scene/Mesh.h"
#include "../Scene/Transform.h"
#include "../Scene/Light.h"
#include "../Scene/Actor.h"
#include "../System/LightManager.h"
#include <iostream>

void PassForward::Init()
{
    std::string json_path = "Assets/Passes_json/pass_forward.json";
    asset = PassAssets::Load(json_path);
    auto vs_src = PassAssets::ReadText(asset->getVSPath());
    auto fs_src = PassAssets::ReadText(asset->getFSPath());
    shader = std::make_shared<Shader>(vs_src, fs_src);

}

void PassForward::Draw(const std::vector<Mesh*>& meshes, const Camera& camera)
{
    shader->use();
    //Camera数据上传
    glm::mat4 V = camera.getViewMatrix();
    glm::mat4 P = camera.getProjectionMatrix();
    glm::vec3 cameraPos = camera.getPosition();
    shader->setVec3("viewPos", cameraPos);
    //光源数据上传
    auto& lightManager = LightManager::Get();
    lightManager.upLoadToShader(shader.get());
    shader->setInt("lightCount", lightManager.getLightCount());
    for (Mesh* mc : meshes)
    {
        Actor* owner = mc->GetOwner();
        auto* trans = owner->GetComponent<Transform>();

        glm::mat4 M = trans->getModelMatrix();
        glm::mat4 MVP = P * V * M;

        shader->setMat4("MVP", MVP);
        shader->setMat4("M", M);
        MeshGPU* gpu_data = mc->getMeshGPU();
        gpu_data->getMaterial()->setShader(shader);
        gpu_data->Bind();
        gpu_data->Draw();
    }
}

void PassForward::ReloadShader(const std::string& vsSrc, const std::string& fsSrc)
{
    shader = std::make_unique<Shader>(vsSrc, fsSrc);
}
