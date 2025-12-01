#pragma once
#include <vector>

class Camera;

class CameraSystem
{
public:
    static CameraSystem& Get();

    // 注册/反注册
    void Register(Camera* cam);
    void UnRegister(Camera* cam);

    // 由 Window 的回调直接调用
    void OnWindowResized(int width, int height);

    // 每帧在应用层调用（main 循环里）
    void UpdateFromInput(float dt);

private:
    CameraSystem() = default;
    std::vector<Camera*> cameras;
};

