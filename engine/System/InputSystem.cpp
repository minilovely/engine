#include "InputSystem.h"

#include "GLFW/glfw3.h"
#include <algorithm>

bool InputSystem::keysCur[512] = {};
bool InputSystem::keysPre[512] = {};
bool InputSystem::mouse[3] = {};
glm::vec2 InputSystem::mouseCurPos = glm::vec2(0.0f);
glm::vec2 InputSystem::mousePrePos = glm::vec2(0.0f);

float InputSystem::scroll_Y = 0.0f;

void InputSystem::Tick()
{
	std::copy(std::begin(keysCur), std::end(keysCur), std::begin(keysPre));
	//mousePrePos = mouseCurPos;
}
bool InputSystem::isKeyPressed(int key)
{
	return keysCur[key] && !keysPre[key];
}
bool InputSystem::isKeyDown(int key)
{
	return keysCur[key] && keysPre[key];
}
bool InputSystem::isKeyReleased(int key)
{
	return !keysCur[key] && keysPre[key];
}
bool InputSystem::isMouseDown(int button)
{
	return mouse[button];
}
float InputSystem::getTotalScrollOffset()
{
	return scroll_Y;
}
void InputSystem::setKey(int key, bool down)
{
	keysCur[key] = down;
}
void InputSystem::setMousePos(float x, float y)
{
	mouseCurPos = glm::vec2(x, y);
}
void InputSystem::setMouseButton(int key, bool down)
{
	mouse[key] = down;
}
void InputSystem::setTotalOffset(float offset)
{
	scroll_Y = offset;
}
void InputSystem::resetScroll()
{
	scroll_Y = 0.0f;
}
