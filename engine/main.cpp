#include "Assets/model.h"
#include "Assets/MeshPrimitives.h"
#include "Assets/ImporterPMX.h"
#include "Render/RenderDevice.h"
#include "RenderPipeline/RenderPipeline.h"
#include "RenderPipeline/PassForward.h"
#include "RenderPiPeline/PassShadow.h"
#include "Scene/Camera.h"
#include "Scene/Actor.h"
#include "Scene/Mesh.h"
#include "Scene/Transform.h"
#include "Scene/Light.h"
#include "System/CameraSystem.h"
#include "System/Window.h"
#include "System/LightManager.h"
#include "Core/MeshRegistry.h"
#include "Utils.h"

#include <memory>
#include <stdexcept>
#include<iostream>

/*
程序食用说明：
1.window窗体创建  example:    Window window(长, 宽, "窗口名");
2.载体创建        example:    auto actor = std::make_unique<Actor>("载体名");
组件：
1.camera创建      example:    auto camComp = actor->AddComponent<Camera>();
2.灯光创建        example:    auto light = actor->AddComponent<Light>();
3.载体坐标创建    example:    auto trans = actor->AddComponent<Transform>();

支持功能：
1.mesh支持自定义添加Pass类型渲染方式
2.mesh支持自定义选择json文件，绑定已有的shader，针对于该mesh的渲染设置
3.场景支持通过控制mesh的属性value,进行自定义渲染顺序
4.场景支持通过RenderDevice控制渲染场景的通用设置，如深度测试，剔除
5.

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
    auto cameraActor = Utils::MakeCameraActor("camera");
    Camera* mainCam = cameraActor->GetComponent<Camera>();

    MeshRegistry& mr = MeshRegistry::instance();
    mr.clear();
    //模型
    std::shared_ptr<PassAssets> model_asset = std::make_shared<PassAssets>();
    model_asset->Load("Assets/Passes_json/model.json");
    auto modelActor = Utils::MakeModelActor("D:/Models/LTY/luotianyi_v4_chibi_ver3.0.pmx",
        "model", model_asset);
    auto model_meshes = modelActor->GetComponents<Mesh>();
    std::shared_ptr<Shader> modelShader = model_asset->getShader();
    auto forwardPass = std::make_shared<PassForward>();
    for (Mesh* m : model_meshes)
    {
        m->getMeshGPU()->getMaterial()->setShader(modelShader);
        m->setValue(1500);
    }
    mr.append(model_meshes);

    //平面
    std::shared_ptr<PassAssets> plane_asset = std::make_shared<PassAssets>();
    plane_asset->Load("Assets/Passes_json/plane.json");
    auto planeActor = Utils::MakePlaneActor("plane", plane_asset);
    auto plane_mesh = planeActor->GetComponent<Mesh>();
    std::shared_ptr<Shader> planeShader = plane_asset->getShader();
    plane_mesh->getMeshGPU()->getMaterial()->setShader(planeShader);
    plane_mesh->setValue(2000);
    mr.add(plane_mesh);

    //光源
    //auto pointLight = Utils::MakePointLightActor("pointLight");
    auto dirLightActor = Utils::MakeDirectionalLightActor("mainLight");
    Light* dirLight = dirLightActor->GetComponent<Light>();
    LightManager::Get().registerLight(dirLightActor.get(), dirLight);

    RenderPipeline pipeline;//这种申请方式会在栈中申请内存
    pipeline.AddPass<PassShadow>();
    pipeline.AddPass<PassForward>();//所有加入pipeline的Pass目前设定执行Init()

    //这个mesh目前为空

    while (!window.shouldClose())
    {
        window.PollEvent();
        CameraSystem::Instance().UpdateFromInput(0.016f);//0.016≈1/60

        RenderDevice::Clear({ 0.4f,0.4f,0.4f });
        RenderDevice::SetDepthTest(true);
        RenderDevice::SetCullEnabled(true);

        pipeline.Render(mr.getallMeshes(), *mainCam);

        window.SwapBuffers();
    }
    return 0;
}
