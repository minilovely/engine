#include "Transform.h"

void Transform::setRotation(const glm::vec3& eulerDegrees)
{
	rotation = glm::quat(glm::radians(eulerDegrees));
}

void Transform::setRotation(const glm::quat& quat)
{
	rotation = quat;
}

glm::vec3 Transform::getEulerAngles() const
{
	return glm::degrees(glm::eulerAngles(rotation));
}

glm::mat4 Transform::getModelMatrix() const
{
	glm::mat4 t = glm::translate(glm::mat4(1), position);
	glm::mat4 r = glm::mat4_cast(rotation);
	glm::mat4 s = glm::scale(glm::mat4(1), scale);
	return t * r * s;
}
