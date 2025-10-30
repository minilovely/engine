#pragma once
#include "../Core/math.h"
#include "Component.h"
class Camera : public Component
{
public:
	explicit Camera(Actor* owner);
	~Camera();

	glm::vec3 getPosition() const { return position; }
	glm::vec3 getCenter() const { return center; }
	glm::vec3 getUp() const { return up; }
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	void setCenter(glm::vec3 newCenter) { center = newCenter; }
	void setUp(glm::vec3 newUp) { up = newUp; }
	void setFov(float f) { fov = f; }
	void setMoveSpeed(float m) { moveSpeed = m; }
	void setZoomSpeed(float z) { zoomSpeed = z; }
	void setNearPlane(float n) { nearPlane = n; }
	void setFarPlane(float f) { farPlane = f; }

	void updateAspect(int width, int height);
	void move(const glm::vec3& offset);
	void rotate(float yaw, float pitch);
	void zoom(float amount);

	void updateFromInput(float dt);

private:
	glm::vec3 position;
	glm::vec3 center;
	glm::vec3 up;

	float fov = 60.0f;
	float aspect = 800.0f / 500.0f;
	float nearPlane = 1.0f;
	float farPlane = 100.0f;

	float moveSpeed = 1.0f;
	//float rotateSpeed = 0.1f;
	float zoomSpeed = 1.0f;

	float yaw = 0.0f;
	float pitch = 0.0f;
	//bool isFirstMouse = true;

	float lastX = 0.f, lastY = 0.f;   // …œ“ª÷° Û±Í
	bool  firstMouse = true;
	void updateCamera();
};

