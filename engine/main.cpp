#include "./Assets/model.h"
#include "./Assets/MeshPrimitives.h"
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
    std::unique_ptr<Actor> MakeModelActor(const std::string& filePath, std::string name)
    {
        auto model = ImporterPMX::Load(filePath);

        auto actor = std::make_unique<Actor>(name);
        auto trans = actor->AddComponent<Transform>();
        trans->setPosition({ 0, 0, 0 });
        trans->setScale({ 0.5f, 0.5f, 0.5f });

        for (const auto& mesh : model->meshes)
        {
            auto meshComp = actor->AddComponent<Mesh>();
            meshComp->transCPUToGPU(mesh);
        }
        return actor;
    }

    std::unique_ptr<Actor> MakeCameraActor(std::string name)
    {
        auto actor = std::make_unique<Actor>(name);
        auto transComp = actor->AddComponent<Transform>();
        transComp->setPosition({ 0, 0, 6 });

        auto camComp = actor->AddComponent<Camera>();
        camComp->setCenter({ 0,3,0 });
        return actor;
    }

    std::unique_ptr<Actor> MakeDirectionalLightActor(std::string name)
    {
        auto actor = std::make_unique<Actor>(name);
        auto trans = actor->AddComponent<Transform>();
        auto light = actor->AddComponent<Light>();

        light->setType(0);  // Directional
        light->setDirection(glm::vec3(-0.5f, -1.0f, -0.5f));
        light->setColor(glm::vec3(1.0f, 0.9f, 0.8f));
        light->setIntensity(1.2f);

        return actor;
    }

    std::unique_ptr<Actor> MakePointLightActor(std::string name)
    {
        auto actor = std::make_unique<Actor>(name);
        auto trans = actor->AddComponent<Transform>();
        auto light = actor->AddComponent<Light>();

        light->setType(1);  // Point
        light->setColor(glm::vec3(0.8f, 0.8f, 1.0f));
        light->setIntensity(3.0f);
        trans->setPosition({ -2.0f, 3.0f, 2.0f });

        return actor;
    }

    std::unique_ptr<Actor> MakePlaneActor(std::string name)
    {
        auto planeActor = std::make_unique<Actor>(name);
        auto trans = planeActor->AddComponent<Transform>();
        trans->setPosition({ 0, 0, 0 });
        trans->setScale({ 10, 1, 10 });//注意：Y轴数据不起作用
        auto mesh_plane = planeActor->AddComponent<Mesh>();
        MeshPrimitives primitive;
        primitive.setColor(glm::vec3(0.6f, 0.6f, 0.6f));
        mesh_plane->transCPUToGPU(primitive.makePlane());
        return planeActor;
    }

    auto loadShader = [](const std::string& jsonPath)
    {
        auto asset = PassAssets::Load(jsonPath);
        return std::make_shared<Shader>(
            PassAssets::ReadText(asset->getVSPath()),
            PassAssets::ReadText(asset->getFSPath())
        );
    };

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
    //窗口
    Window window(1280, 720, "Model Viewer");

    //相机
    auto cameraActor = MakeCameraActor("camera");
    Camera* mainCam = cameraActor->GetComponent<Camera>();

    std::vector<Mesh*> allMeshes;

    //模型
    auto modelActor = MakeModelActor("D:/Models/LTY/luotianyi_v4_chibi_ver3.0.pmx","model");
    auto model_meshes = modelActor->GetComponents<Mesh>();
    std::shared_ptr<Shader> modelShader = loadShader("Assets/Passes_json/model.json");
    auto forwardPass = std::make_shared<PassForward>();
    for (Mesh* m : model_meshes)
    {
        m->getMeshGPU()->getMaterial()->setShader(modelShader);
        m->addPass(forwardPass);
    }
    allMeshes.insert(allMeshes.begin(), model_meshes.begin(), model_meshes.end());
    //平面
    auto planeActor = MakePlaneActor("plane");
    auto plane_mesh = planeActor->GetComponent<Mesh>();
    std::shared_ptr<Shader> planeShader = loadShader("Assets/Passes_json/plane.json");
    plane_mesh->getMeshGPU()->getMaterial()->setShader(planeShader);
    plane_mesh->addPass(forwardPass);
    allMeshes.push_back(plane_mesh);

    //光源
    auto pointLight = MakePointLightActor("pointLight");

    RenderPipeline pipeline;//这种申请方式会在栈中申请内存

    pipeline.AddPass<PassForward>();//所有加入pipeline的Pass目前设定执行Init()

    //这个mesh目前为空

    while (!window.shouldClose())
    {
        window.PollEvent();
        CameraSystem::Instance().UpdateFromInput(0.016f);//0.016≈1/60

        RenderDevice::Clear({ 0.2f,0.3f,0.3f });
        RenderDevice::SetDepthTest(true);

        pipeline.Render(allMeshes, *mainCam);

        window.SwapBuffers();
    }
    return 0;
}
