#include "RenderQueue.h"
#include "../System/LightManager.h"
#include "../Render/RenderDevice.h"
#include "../Core/math.h"

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

void RenderQueue::DrawShadow()
{
    if (!shadowMap)
    {
        std::cout << "shadowMap is null" << std::endl;
        return;
    }
    shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    RenderDevice::SetCullEnabled(false);

    for (const auto& cmd : cmds)
    {
        if (cmd.PassType != RenderCommand::PassType::Shadow) continue;

        auto shader = cmd.shadowAssets ? cmd.shadowAssets->getShader() : nullptr;
        if (shader)
        {
            shader->use();
            shader->setMat4("lightSpaceMatrix", cmd.lightSpaceMatrix);
            shader->setMat4("M", cmd.M);
        }

        if (cmd.mesh)
        {
            cmd.mesh->Bind();
            RenderDevice::SetDepthWrite(true);
            RenderDevice::SetColorWrite(false);
            cmd.mesh->Draw();
        }
    }

    shadowMap->Unbind();
    RenderDevice::SetCullEnabled(true);
}

void RenderQueue::DrawForward()
{
    RenderDevice::SetCullEnabled(false);

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

        shader->setMat4("lightSpaceMatrix", cmd.lightSpaceMatrix);
        shader->setInt("shadowMap", 1);
        if (shadowMap)
        {
            shadowMap->BindTexture(1);
        }
        else
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        cmd.mesh->Bind();
        RenderDevice::SetDepthWrite(cmd.depthWrite);
        RenderDevice::SetColorWrite(cmd.colorWrite);
        RenderDevice::SetCullMode(cmd.cullMode);
        cmd.mesh->Draw();
    }
}


