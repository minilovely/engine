#include "PassForward.h"
#include "../Scene/Camera.h"
#include "../Scene/Mesh.h"
#include "../Scene/Transform.h"
#include "../Scene/Actor.h"
#include "../RenderPiPeline/RenderQueue.h"
#include "../System/LightManager.h"

#include <iostream>

void PassForward::Init()
{
    
}

void PassForward::Collect(const Camera& camera,Mesh* mesh, RenderQueue& outQueue)
{
    SkeletonPose* pose = GlobalSkeletonCache::get().getPose(mesh->GetOwner()->getName());
    auto owner = mesh->GetOwner();
    auto trans = owner->GetComponent<Transform>();
    auto gpuMesh = mesh->getMeshGPU();
    auto mat = gpuMesh->getMaterial();

    auto lightManager = &LightManager::Get();
    Light* dirLight = lightManager ? lightManager->GetMainDirectionalLight() : nullptr;
    if (!dirLight) return;
    glm::mat4 lightSpaceMatrix = dirLight->GetLightSpaceMatrix();

    const glm::mat4 V = camera.getViewMatrix();
    const glm::mat4 P = camera.getProjectionMatrix();
    const glm::vec3 camPos = camera.getPosition();

    RenderCommand cmd;
    cmd.mesh = gpuMesh;
    cmd.M = trans->getModelMatrix();
    cmd.MVP = P * V * cmd.M;
	cmd.VP = P * V;
    cmd.viewPos = camPos;
    cmd.material = mat;
    cmd.value = mesh->getValue();
    cmd.PassType = RenderCommand::PassType::Forward;
    cmd.depthWrite = mesh->getDepthWrite();
    cmd.colorWrite = mesh->getColorWrite();
    cmd.cullMode = mesh->getCullMode();
    cmd.lightSpaceMatrix = lightSpaceMatrix;
    if (pose)
    {
		cmd.hasBones = true;
		cmd.uBoneMats.reserve(gpuMesh->localBonesIdx.size());
        for(auto idx : gpuMesh->localBonesIdx)
        {
            if (idx >= 0 && idx < pose->finalMatrices.size())
            {
                cmd.uBoneMats.push_back(pose->finalMatrices[idx]);
            }
            else
            {
                std::cerr << "[PassForward] Invalid global bone index: " << idx << std::endl;
                cmd.uBoneMats.push_back(glm::mat4(1.0f));
            }
		}
    }
    outQueue.Add(cmd);
}
