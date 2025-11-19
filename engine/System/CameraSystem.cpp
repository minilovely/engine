#include "CameraSystem.h"
#include "../Scene/Camera.h"

CameraSystem& CameraSystem::Instance()
{
    static CameraSystem inst;
    return inst;
}

void CameraSystem::Register(Camera* cam) 
{
    cameras.push_back(cam);
}
void CameraSystem::UnRegister(Camera* cam)
{
    cameras.erase(std::remove(cameras.begin(), cameras.end(), cam), cameras.end());
}

void CameraSystem::OnWindowResized(int w, int h)
{
    for (Camera* c : cameras) c->updateAspect(w, h);
}

void CameraSystem::UpdateFromInput(float dt)
{
    for (Camera* c : cameras) c->updateFromInput(dt);
}
