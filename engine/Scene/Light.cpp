#include "Light.h"
#include "../Render/Shader.h"
#include "Actor.h"
#include "Transform.h"

#include <string>
#include <iostream>
//Light::Light(Actor* owner,LightType type,
//				glm::vec3 color,float intensity)
//	
//{
//	switch (type)
//	{
//	case LightType::Diractional:
//		direction = glm::vec3(0, 1, 0);
//		break;
//	case LightType::Point:
//		range = 10.0f;
//		break;
//	}
//}

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

void Light::setDirection(glm::vec3 dirPos)
{
    glm::vec3 startPos = getPos();
    glm::vec3 dir = dirPos - startPos;
    direction = glm::normalize(dir);
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
