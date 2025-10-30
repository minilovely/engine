#pragma once
#include "Pass.h"
#include "../RenderPiPeline/RenderQueue.h"
#include "../Scene/Mesh.h"

#include <vector>
#include <memory>

class RenderPipeline
{
public:
    template<typename T, typename... Args>
    void AddPass(Args&&... args)
    {
        auto pass = std::make_shared<T>(std::forward<Args>(args)...);
        pass->Init();
        passes.push_back(pass);
    }

    void Render(const std::vector<Mesh*>& meshes, const Camera& cam)
    {
        queue.Clear();
        for (auto& pass : passes)
        {
            for (auto* mesh : meshes)
            {
                pass->Collect(cam, mesh, queue);
            }
        }
        queue.Sort();
        queue.DrawShadow(); // 首先绘制阴影贴图
        queue.DrawForward(); // 然后主渲染采样
    }

private:
    RenderQueue queue;
    std::vector<std::shared_ptr<Pass>> passes;
};