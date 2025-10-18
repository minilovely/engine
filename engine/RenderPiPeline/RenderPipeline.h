#pragma once
#include <vector>
#include <memory>
#include "Pass.h"
#include "../RenderPiPeline/RenderQueue.h"

class RenderPipeline
{
public:
    template<typename T, typename... Args>
    void AddPass(Args&&... args)
    {
        auto pass = std::make_unique<T>();
        pass->Init();
        passes.push_back(std::move(pass));
    }

    void Render(const std::vector<Mesh*>& meshes, const Camera& cam)
    {
        queue.Clear();
        for (auto& pass : passes)
            pass->Collect(cam, meshes, queue);
        queue.Sort();
        queue.Draw();
    }

private:
    std::vector<std::unique_ptr<Pass>> passes;
    RenderQueue queue;
};