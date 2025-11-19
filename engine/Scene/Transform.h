#pragma once
#include "../Core/math.h"
#include "Component.h"
class Transform : public Component
{
public:
	//在Transform的构造函数替换为调用基类构造函数
	Transform(Actor* owner) : Component(owner){}
	void setPosition(const glm::vec3& p) { position = p; }

	void setRotation(const glm::vec3& eulerDegrees);
	void setRotation(const glm::quat& quat);

	void setScale(const glm::vec3& s) { scale = s; }

	const glm::quat& getRotation() const { return rotation; }
	const glm::vec3& getPosition() const { return position; }
	const glm::vec3& getScale() const { return scale; }

	glm::vec3 getEulerAngles() const;
	glm::mat4 getModelMatrix() const;
private:
	glm::vec3 position{ 0 };
	glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1 };
};

