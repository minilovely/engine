#include "./Assets/model.h"
#include "Render/RenderDevice.h"
#include "RenderPipeline/RenderPipeline.h"
#include "RenderPipeline/PassForward.h"
#include "./Scene/Camera.h"
#include "./Render/MeshGPU.h"
#include "Assets/ImporterPMX.h"
#include "Scene/Actor.h"
#include "Scene/Mesh.h"
#include "Scene/Transform.h"
#include "Scene/Light.h"
#include "System/CameraSystem.h"
#include "System/Window.h"
#include "System/LightManager.h"
#include <memory>
#include <stdexcept>
#include<iostream>

namespace 
{   
    // 匿名命名空间，内部链接
    std::unique_ptr<Actor> MakeModelActor(const std::string& filePath)
    {
        auto model = ImporterPMX::Load(filePath);
        if (!model) throw std::runtime_error("load model failed!");

        auto actor = std::make_unique<Actor>("Model");
        auto trans = actor->AddComponent<Transform>();
        trans->setPosition({ 0, 0, 0 });
        trans->setScale({ 0.5f, 0.5f, 0.5f });

        for (const auto& cpu : model->meshes)
        {
            auto meshComp = actor->AddComponent<Mesh>();
            meshComp->SetSingleMesh(cpu);   // 仅上传顶点/索引/贴图
        }
        return actor;
    }

    std::unique_ptr<Actor> MakeCameraActor()
    {
        auto actor = std::make_unique<Actor>("MainCamera");
        auto transComp = actor->AddComponent<Transform>();
        transComp->setPosition({ 0, 0, 6 });

        auto camComp = actor->AddComponent<Camera>();
        camComp->setCenter({ 0,3,0 });
        return actor;
    }
    std::unique_ptr<Actor> MakeDirectionalLightActor()
    {
        auto actor = std::make_unique<Actor>("DirectionalLight");
        auto trans = actor->AddComponent<Transform>();
        auto light = actor->AddComponent<Light>();

        light->setType(0);  // Directional
        light->setDirection(glm::vec3(-0.5f, -1.0f, -0.5f));
        light->setColor(glm::vec3(1.0f, 0.9f, 0.8f));
        light->setIntensity(1.2f);

        return actor;
    }

    std::unique_ptr<Actor> MakePointLightActor()
    {
        auto actor = std::make_unique<Actor>("PointLight");
        auto trans = actor->AddComponent<Transform>();
        auto light = actor->AddComponent<Light>();

        light->setType(1);  // Point
        light->setColor(glm::vec3(0.8f, 0.8f, 1.0f));
        light->setIntensity(3.0f);
        trans->setPosition({ -2.0f, 3.0f, 2.0f });

        return actor;
    }
}


/*
程序食用说明：
1.window窗体创建  example:    Window window(长, 宽, "窗口名");
2.载体创建        example:    auto actor = std::make_unique<Actor>("载体名");
组件：
1.camera创建      example:    auto camComp = actor->AddComponent<Camera>();
2.灯光创建        example:    auto light = actor->AddComponent<Light>();
3.载体坐标创建    example:    auto trans = actor->AddComponent<Transform>();

注意事项:
1.程序所有实体按照——单个载体可对应多组件的形式存在
2.为了保证Transform组件功能的集中性，Camera组件在一定程度上依赖于Transform组件，
  在添加Camera组件之前，需要先添加Transform组件

组件参数修改：
所有组件的参数修改规则：
组件变量名->参数名(修改的数值)

组件属性表：
组件名:        属性名             含义                  其他
Camera
               fov                视角度
               aspect             视口裁切比            暂未开放此项，目前裁切比与窗口大小耦合
               nearPlane          近平面距离
               farPlane           远平面距离
               moveSpeed          移动速度
               zoomSpeed          缩放速度
Transform
               position           载体位置
               rotation           旋转位置
               scale              缩放大小
Light
               type               光源类型              0为平行光，1为点光源
               color              光源颜色
               intensity          光源强度
               direction          平行光方向
               range              点光源范围
               constant           点光源近距离衰减      0.0R~0.3R
               linear             点光源中距离衰减      0.3R~0.7R
               quadratic          点光源远距离衰减      0.7R~1.0R
*/


int main()
{
    /* ---- 窗口 ---- */
    Window window(1280, 720, "Model Viewer");

    /* ---- 相机 ---- */
    auto cameraActor = MakeCameraActor();
    Camera* mainCam = cameraActor->GetComponent<Camera>();

    /* ---- 模型 ---- */
    auto modelActor = MakeModelActor("D:/Models/LTY/luotianyi_v4_chibi_ver3.0.pmx");
    auto meshes = modelActor->GetComponents<Mesh>();

    /* ---- 光源 ---- */
    //auto directionalLight = MakeDirectionalLightActor();
    auto pointLight = MakePointLightActor();
    std::cout << "[Main] Rendering " << meshes.size() << " Mesh components\n";

    /* ---- 渲染管线 ---- */
    RenderPipeline pipeline;//这种申请方式会在栈中申请内存
    pipeline.AddPass<PassForward>();   // 前向光照 Pass

    /* ---- 主循环 ---- */
    while (!window.shouldClose())
    {
        window.PollEvent();
        CameraSystem::Instance().UpdateFromInput(0.016f);//0.016≈1/60

        RenderDevice::Clear({ 0.2f,0.3f,0.3f });
        RenderDevice::SetDepthTest(true);

        // 把 mesh 扔给管线，用 mainCam 绘制
        pipeline.Render(meshes, *mainCam);

        window.SwapBuffers();
    }
    return 0;
}


/*
    目前来说，项目的大概的地基已经完成，但是这块地基还存在着一些问题。
    首当其冲的是关于Passforward这给类的处理，现在它负责的是单个物体的shader编写和UBO的绑定，
    但是当场景中是多物体的渲染的时候，这个类的名字和它的作用就严重的不匹配了，需要重构。
*/