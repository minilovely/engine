#include "PassForward.h"
#include "../Scene/Camera.h"
#include "../Scene/Mesh.h"
#include "../Scene/Transform.h"
#include "../Scene/Actor.h"
#include "../RenderPiPeline/RenderQueue.h"
#include <iostream>

void PassForward::Init()
{
    
}

void PassForward::Collect(const Camera& camera,Mesh* mesh, RenderQueue& outQueue)
{
    const glm::mat4 V = camera.getViewMatrix();
    const glm::mat4 P = camera.getProjectionMatrix();
    const glm::vec3 camPos = camera.getPosition();

    auto* owner = mesh->GetOwner();
    auto* trans = owner->GetComponent<Transform>();
    auto* gpuMesh = mesh->getMeshGPU();
    auto  mat = gpuMesh->getMaterial();

    RenderCommand cmd;
    cmd.mesh = gpuMesh;
    cmd.M = trans->getModelMatrix();
    cmd.MVP = P * V * cmd.M;
    cmd.viewPos = camPos;
    cmd.material = mat;
    cmd.value = mesh->getValue();
    cmd.PassType = RenderCommand::PassType::Forward;
    cmd.depthWrite = mesh->getDepthWrite();
    cmd.colorWrite = mesh->getColorWrite();
    cmd.cullMode = mesh->getCullMode();
    outQueue.Add(cmd);
}