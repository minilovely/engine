#include "Light.h"
#include "../Render/Shader.h"
#include "Actor.h"
#include "Transform.h"

#include <string>
#include <iostream>

Light::~Light()
{
}

glm::vec3 Light::getPos() const
{
    if (auto*tran = owner->GetComponent<Transform>())
    {
        return tran->getPosition();
    }
    else
    {
        std::cout << owner->getName() << "didn't have 'Transform' component" << std::endl;
    }
}

void Light::setDirection(glm::vec3 dir)
{
    direction = glm::normalize(dir);
    updateLightSpaceMatrix();
}

glm::mat4 Light::GetLightSpaceMatrix() const
{
    return lightSpaceMatrix;
}

void Light::updateLightSpaceMatrix()
{
    if(type == static_cast<int>(Type::Diractional))
    {
        glm::vec3 lightDir = glm::normalize(direction);
        glm::vec3 sceneCenter = glm::vec3(0.0f);
        float distance = 80.0f;  // 足够远
        glm::vec3 virtualEye = sceneCenter - lightDir * distance;
        
        glm::mat4 lightView = glm::lookAt(virtualEye, sceneCenter, glm::vec3(0, 1, 0));

        float orthoWidth = 10.0f;
        float nearPlane = 0.1f, farPlane = 100.0f;
        glm::mat4 lightProj = glm::ortho(-orthoWidth, orthoWidth, -orthoWidth, orthoWidth, nearPlane, farPlane);

        lightSpaceMatrix = lightProj * lightView;
    }
    else
    {
        lightSpaceMatrix = glm::mat4(1.0f);
        std::cout << "Diractional light is null" << std::endl;
    }
}

void Light::UploadToShader(Shader* shader,int index)
{
	std::string prefix = "lights[" + std::to_string(index) + "].";

    shader->setVec3(prefix + "position", getPos());
    shader->setVec3(prefix + "direction", direction);
    shader->setVec3(prefix + "color", color * intensity);
    shader->setInt(prefix + "type", static_cast<int>(type));
    shader->setFloat(prefix + "constant", constant);
    shader->setFloat(prefix + "linear", linear);
    shader->setFloat(prefix + "quadratic", quadratic);
    shader->setFloat(prefix + "range", range);
}
