#include "Camera.h"
#include "../System/Input.h"
#include "GLFW/glfw3.h"
#include "../System/Window.h"
#include "../System/CameraSystem.h"
#include "Actor.h"
#include "Transform.h"
Camera::Camera(Actor* owner) : Component(owner)
{
    CameraSystem::Instance().Register(this);
    position = owner->GetComponent<Transform>()->getPosition();
    center = glm::vec3(0, 3, 0);
    up = glm::vec3(0, 1, 0);
    updateCamera();
}

Camera::~Camera()
{
    CameraSystem::Instance().UnRegister(this);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, center, up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::updateAspect(int width, int height)
{
    aspect = static_cast<float>(width) / static_cast<float>(height);
}

void Camera::move(const glm::vec3& offset)
{
    position += offset;
    center += offset;

}

void Camera::rotate(float yaw, float pitch)
{
    this->yaw += yaw;
    this->pitch += pitch;

    // 限制俯仰角
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // 计算新方向
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    center = position + glm::normalize(direction);
}

void Camera::zoom(float amount)
{
    glm::vec3 forward = glm::normalize(center - position);
    position += forward * amount;
    center += forward * amount;
}

void Camera::updateFromInput(float dt)
{
    float m_speed = moveSpeed * dt;
    float z_speed = zoomSpeed * dt;
	glm::vec3 forward = glm::normalize(center - position);
	glm::vec3 right = glm::normalize(glm::cross(forward, up));
    
    if (Input::isKeyDown(GLFW_KEY_W))
    {
        move(forward * m_speed);
    }
    if (Input::isKeyDown(GLFW_KEY_S))
    {
        move(-forward * m_speed);
    }
    if (Input::isKeyDown(GLFW_KEY_A)) 
    {
        move(-right * m_speed);
    }
    if (Input::isKeyDown(GLFW_KEY_D)) 
    {
        move(right * m_speed);
    }
    if (Input::isKeyDown(GLFW_KEY_C)) 
    {
        move(-up * m_speed);
    }
    if (Input::isKeyDown(GLFW_KEY_SPACE)) 
    {
        move(up * m_speed);
    }
    glm::vec2 current = Input::getMousePos();      // 当前 GLFW 坐标
    if (Input::isMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
    {
        if (firstMouse)          // 第一帧只记录，不累加
        {
            lastX = current.x;
            lastY = current.y;
            firstMouse = false;
            return;
        }
        float offsetX = current.x - lastX;
        float offsetY = lastY - current.y;   // 注意 Y 要反转
        lastX = current.x;
        lastY = current.y;

        const float sensitivity = 0.1f;
        yaw += offsetX * sensitivity;
        pitch += offsetY * sensitivity;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 dir;
        dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        dir.y = sin(glm::radians(pitch));
        dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        center = position + glm::normalize(dir);   // 立即刷新
    }
    else
    {
        firstMouse = true;   // 右键一松就重置
    }

    // 滚轮缩放控制
    float scroll = Input::getTotalScrollOffset();
    if (scroll != 0.0f) 
    {
        zoom(scroll * z_speed);
        Input::resetScroll();
    }
}

void Camera::updateCamera()
{
    glm::vec3 direction = glm::normalize(center - position);
    pitch = glm::degrees(asin(direction.y));
    yaw = glm::degrees(atan2(direction.z, direction.x));
}
