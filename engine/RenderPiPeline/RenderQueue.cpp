#include "RenderQueue.h"
#include "../System/LightManager.h"
#include "../Render/RenderDevice.h"
#include <algorithm>

void RenderQueue::Clear()
{
    cmds.clear();
}

void RenderQueue::Add(const RenderCommand& cmd)
{
    cmds.push_back(cmd);
}

void RenderQueue::Sort()
{
    std::stable_sort(cmds.begin(), cmds.end(),
        [](const auto& a, const auto& b) {return a.value < b.value; });
}

void RenderQueue::DrawForward()
{
    auto& lightManager = LightManager::Get();
    for (const auto& cmd : cmds)
    {
        if (cmd.PassType != RenderCommand::PassType::Forward) continue;
        cmd.material->bindTextures();
        auto shader = cmd.material->getShader();
        shader->use();
        lightManager.upLoadToShader(shader.get());
        shader->setMat4("MVP", cmd.MVP);
        shader->setMat4("M", cmd.M);
        shader->setVec3("viewPos", cmd.viewPos);
        shader->setVec3("color", cmd.material->getTexColor());
        shader->setInt("shadowMap", 1);
        shadowMap->BindTexture(1);
        cmd.mesh->Bind();
        RenderDevice::SetDepthWrite(cmd.depthWrite);
        RenderDevice::SetColorWrite(cmd.colorWrite);
        RenderDevice::SetCullMode(cmd.cullMode);
        cmd.mesh->Draw();
    }
}

void RenderQueue::DrawShadow()
{
    for (const auto& cmd : cmds)
    {
        if (cmd.PassType != RenderCommand::PassType::Shadow) continue;
        shadowMap = cmd.shadowMap;
        shadowMap->Bind();
        auto shader = cmd.material ? cmd.material->getShader() : nullptr;
        if (shader)
        {
            shader->use();
            shader->setMat4("lightSpaceMatrix", cmd.MVP);
            shader->setMat4("M", cmd.M);
        }
        if (cmd.mesh)
        {
            cmd.mesh->Bind();
            RenderDevice::SetDepthWrite(true);
            RenderDevice::SetColorWrite(false);
            RenderDevice::SetCullMode("Back");
            cmd.mesh->Draw();
        }
    }
    shadowMap->Unbind();
}


