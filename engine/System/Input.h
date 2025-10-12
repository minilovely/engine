#pragma once
#include "../Core/math.h"
//
class Input
{
	friend class Window;
public:
	//将当前输入状态复制到上一帧数据
	static void Tick();
	//keybored
	//短按键
	static bool isKeyPressed(int key);
	//长按键
	static bool isKeyDown(int key);
	static bool isKeyReleased(int key);
	//mouse
	static bool isMouseDown(int button);
	//static glm::vec2 getMouseOffset();
	static glm::vec2 getMousePos() { return mouseCurPos; }
	static float getTotalScrollOffset();
	static void resetScroll();

private:
	static bool keysCur[512];
	static bool keysPre[512];
	static bool mouse[3];
	static glm::vec2 mouseCurPos;
	static glm::vec2 mousePrePos;
	//一帧当中的偏移量
	static float scroll_Y;

	static void setKey(int key, bool down);
	static void setMousePos(float x, float y);
	static void setMouseButton(int key, bool down);
	static void setTotalOffset(float offset);
};



