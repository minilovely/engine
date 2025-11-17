#pragma once
#include "Scene/Mesh.h"
#include "Scene/Actor.h"
#include "Scene/Transform.h"
#include "Scene/Camera.h"
#include "RenderPiPeline/PassForward.h"
#include "Assets/ImporterPMX.h"
#include "Assets/MeshPrimitives.h"
#include "Assets/Bone.h"
#include "System/CameraSystem.h"
#include "Render/SkyBox.h"
#include "Scene/VmdAnimation.h"

#include <memory>
#include <vector>

namespace Utils
{
    std::unique_ptr<Actor> MakeModelActor(const std::string& filePath, std::string name, std::shared_ptr<PassAssets> asset)
    {
        auto actor = std::make_unique<Actor>(name);
        
        auto model = ImporterPMX::Load(filePath);

        auto trans = actor->AddComponent<Transform>();
        trans->setPosition({ 0, 0, 0 });
        trans->setScale({ 0.5f, 0.5f, 0.5f });
		trans->setRotation({ 0, 180, 0 });

        GlobalSkeletonCache::get().registerSkeleton(name, model->skeleton);
        GlobalSkeletonCache::get().createPose(name, model->skeleton.bones.size());//目前是空姿势表
        //vmd动画组件
        auto* vmd = actor->AddComponent<VmdAnimation>();
        vmd->Load("D:/Models/t1.vmd");  
        vmd->Play();

        for (const auto& mesh : model->meshes)
        {
            auto meshComp = actor->AddComponent<Mesh>();
            meshComp->transCPUToGPU(mesh);
            meshComp->setDepthWrite(asset->getDepthWrite());
            meshComp->setColorWrite(asset->getColorWrite());
            meshComp->setCullMode(asset->getCullMode());
        }
        return actor;
    }

    std::unique_ptr<Actor> MakeCameraActor(std::string name)
    {
        auto actor = std::make_unique<Actor>(name);
        auto transComp = actor->AddComponent<Transform>();
        transComp->setPosition({ 0, 5, 6 });

        auto camComp = actor->AddComponent<Camera>();
        camComp->setCenter({ 0,3,0 });
        return actor;
    }

    std::unique_ptr<Actor> MakeDirectionalLightActor(std::string name)
    {
        auto actor = std::make_unique<Actor>(name);
        auto trans = actor->AddComponent<Transform>();
        auto light = actor->AddComponent<Light>();

        trans->setPosition(glm::vec3(2.0, 0.0, 0.0));
        light->setType(0);  // Directional
        light->setDirection(glm::vec3(1.0f, 0.0f, -1.0f));
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

    std::unique_ptr<Actor> MakePlaneActor(std::string name, std::shared_ptr<PassAssets> asset)
    {
        auto planeActor = std::make_unique<Actor>(name);
        auto trans = planeActor->AddComponent<Transform>();
        trans->setPosition({ 0, 0, -3 });
        trans->setScale({ 100, 1, 100 });//注意：Y轴数据不起作用
        auto mesh_plane = planeActor->AddComponent<Mesh>();
        MeshPrimitives primitive;
        primitive.setColor(glm::vec3(0.6f, 0.6f, 0.6f));
        mesh_plane->transCPUToGPU(primitive.makePlane());
        mesh_plane->setDepthWrite(asset->getDepthWrite());
        mesh_plane->setColorWrite(asset->getColorWrite());
        mesh_plane->setCullMode(asset->getCullMode());
        return planeActor;
    }
    std::unique_ptr<SkyBox> MakeSkyBox(std::vector<std::string> faces)
    {
        auto skyBox = std::make_unique<SkyBox>();
        skyBox->Init();
        auto cubeMap = std::make_shared<TextureCube>(faces);
        skyBox->SetCubeMap(cubeMap);
        return skyBox;
    }

}
