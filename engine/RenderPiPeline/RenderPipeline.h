#pragma once
#include <vector>
#include <memory>
#include "Pass.h"
#include "../RenderPiPeline/RenderQueue.h"
#include "../Scene/Mesh.h"

class RenderPipeline
{
public:
    template<typename T, typename... Args>
    void AddPass(Args&&... args)
    {
        auto pass = std::make_unique<T>();
        pass->Init();
    }

    void Render(const std::vector<Mesh*>& meshes, const Camera& cam)
    {
        queue.Clear();
        for (Mesh* mc : meshes)
        {
            for (auto& pass : mc->getPasses())
            {
                pass->Collect(cam, mc, queue);
            }
        }
        queue.Sort();
        queue.Draw();
    }

private:
    RenderQueue queue;
};