#pragma once
#include "Scene/Mesh.h"
#include "Scene/Actor.h"
#include "RenderPiPeline/PassForward.h"
#include "Assets/ImporterPMX.h"
#include "Scene/Transform.h"
#include "System/CameraSystem.h"
#include "./Scene/Camera.h"
#include "./Assets/MeshPrimitives.h"

#include <memory>
#include <vector>

class Utils
{
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
        trans->setScale({ 10, 1, 10 });
        auto mesh_plane = planeActor->AddComponent<Mesh>();
        mesh_plane->transCPUToGPU(MeshPrimitives::makePlane());
        return planeActor;
    }

    auto loadShader = [](const std::string& jsonPath)
        {
            auto asset = PassAssets::Load(jsonPath);
            return std::make_shared<Shader>(
                PassAssets::ReadText(asset->getVSPath()),
                PassAssets::ReadText(asset->getFSPath()));
        };

    void meshSetShader()

};