#pragma once
#include <vector>

class VmdAnimation;

class AnimationSystem
{
public:
    static AnimationSystem& Get();

    // 注册/反注册动画组件
    void Register(VmdAnimation* animation);
    void UnRegister(VmdAnimation* animation);

    void UpdateFromInput(float dt);

private:
    AnimationSystem();
    std::vector<VmdAnimation*> animations;
    bool wasVKeyPressed; // 用于防止重复触发
};
