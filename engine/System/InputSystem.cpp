#include "InputSystem.h"

#include "GLFW/glfw3.h"
#include <algorithm>

bool Input::keysCur[512] = {};
bool Input::keysPre[512] = {};
bool Input::mouse[3] = {};
glm::vec2 Input::mouseCurPos = glm::vec2(0.0f);
glm::vec2 Input::mousePrePos = glm::vec2(0.0f);

float Input::scroll_Y = 0.0f;

void Input::Tick()
{
	std::copy(std::begin(keysCur), std::end(keysCur), std::begin(keysPre));
	//mousePrePos = mouseCurPos;
}
bool Input::isKeyPressed(int key)
{
	return keysCur[key] && !keysPre[key];
}
bool Input::isKeyDown(int key)
{
	return keysCur[key] && keysPre[key];
}
bool Input::isKeyReleased(int key)
{
	return !keysCur[key] && keysPre[key];
}
bool Input::isMouseDown(int button)
{
	return mouse[button];
}
//glm::vec2 Input::getMouseOffset()
//{
//	return glm::vec2(mouseCurPos - mousePrePos);
//}
float Input::getTotalScrollOffset()
{
	return scroll_Y;
}
void Input::setKey(int key, bool down)
{
	keysCur[key] = down;
}
void Input::setMousePos(float x, float y)
{
	mouseCurPos = glm::vec2(x, y);
}
void Input::setMouseButton(int key, bool down)
{
	mouse[key] = down;
}
void Input::setTotalOffset(float offset)
{
	scroll_Y = offset;
}
void Input::resetScroll()
{
	scroll_Y = 0.0f;
}
