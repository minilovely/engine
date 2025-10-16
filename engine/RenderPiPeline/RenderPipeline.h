#pragma once
#include <vector>
#include <memory>
#include "Pass.h"
//#include "../Assets/PassAssets.h"

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

    // 直接传“可见网格列表”+相机
    void Render(const std::vector<Mesh*>& meshes,
        const Camera& cam)
    {
        for (auto& p : passes) p->Draw(meshes, cam);
    }

private:
    std::vector<std::unique_ptr<Pass>> passes;
};