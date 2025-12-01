#pragma once
#include "../Core/math.h"
#include "../Render/ShadowMap.h"
#include "Component.h"

#include <memory>
class Shader;

class Light : public Component
{
public:
	enum class Type : int
	{
		Directional = 0,
		Point = 1
	};

	explicit Light(Actor* owner);

	Light() = default;
	~Light();

	glm::vec3 getPos() const;
	glm::vec3 getColor() const { return color; }
	float getIntensity() const { return intensity; }
	int getType() const { return type; }
	glm::vec3 getDirection() const { return direction; }
	float getRange() const { return range; }
	glm::mat4 GetLightSpaceMatrix() const;

	void setColor(glm::vec3 col) { this->color = col; }
	void setIntensity(float i) { this->intensity = i; }
	void setType(int t) { type = t; }
	void setDirection(glm::vec3 dir);
	void setRange(float r) { range = r; }
	void setConstant(float c) { constant = c; }
	void setLinear(float li) { linear = li; }
	void setQuadratic(float q) { quadratic = q; }

	void UploadToShader(Shader* shader,int i);


private:
	glm::vec3 color = glm::vec3(0.8, 0.8, 0.8);
	float intensity = 2.0f;
	int type;
	std::unique_ptr<ShadowMap> shadowMap;
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f); // 仅平行光生效，点光源等为单位阵
	glm::vec3 direction;

	//point light
	float range = 10.0f;
	float constant = 1.0f;
	float linear = 0.35f;
	float quadratic = 0.44f;
	void updateLightSpaceMatrix();
};

