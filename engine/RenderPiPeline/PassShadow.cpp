#include "PassShadow.h"
#include "../Scene/Camera.h"
#include "../Scene/Mesh.h"
#include "../Scene/Transform.h"
#include "../RenderPiPeline/RenderQueue.h"
#include "../Scene/Light.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad.h>
#include <iostream>

PassShadow::PassShadow(){}

void PassShadow::Init()
{
    lightManager = &LightManager::Get();
    if (!shadowMap)
        shadowMap = std::make_shared<ShadowMap>();
    shadowMap->Init(2048);
    shadowAssets = std::make_shared<PassAssets>();
    shadowAssets->Load("Assets/Passes_json/shadow.json");
}

void PassShadow::Collect(const Camera& /*camera*/, Mesh* mesh, RenderQueue& outQueue)
{
	SkeletonPose* pose = GlobalSkeletonCache::get().getPose(mesh->GetOwner()->getName());
    // 获取主平行光
    Light* dirLight = lightManager ? lightManager->GetMainDirectionalLight() : nullptr;
    if (!dirLight) return;
    glm::mat4 lightSpaceMatrix = dirLight->GetLightSpaceMatrix();
    auto* owner = mesh->GetOwner();
    auto* trans = owner->GetComponent<Transform>();
    auto* gpuMesh = mesh->getMeshGPU();
    auto* mat = gpuMesh->getMaterial();
    RenderCommand cmd;
    cmd.mesh = gpuMesh;
    cmd.material = mat;
    cmd.M = trans ? trans->getModelMatrix() : glm::mat4(1.0f);
    cmd.lightSpaceMatrix = lightSpaceMatrix;
    cmd.shadowMap = shadowMap;
    cmd.shadowAssets = shadowAssets;
    cmd.PassType = RenderCommand::PassType::Shadow;
	if (pose)
	{
		cmd.hasBones = true;
		cmd.uBoneMats.reserve(gpuMesh->localBonesIdx.size());
		for (auto idx : gpuMesh->localBonesIdx)
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

