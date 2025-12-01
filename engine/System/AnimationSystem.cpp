#include "AnimationSystem.h"
#include "InputSystem.h"
#include "../Scene/VmdAnimation.h"
#include "GLFW/glfw3.h"

#include <iostream>

AnimationSystem::AnimationSystem()
{
    wasVKeyPressed = false;
}

AnimationSystem& AnimationSystem::Get()
{
    static AnimationSystem inst;
    return inst;
}

void AnimationSystem::Register(VmdAnimation* animation)
{
    animations.push_back(animation);
}

void AnimationSystem::UnRegister(VmdAnimation* animation)
{
    animations.erase(std::remove(animations.begin(), animations.end(), animation), animations.end());
}

void AnimationSystem::UpdateFromInput(float dt)
{
    bool vKeyDown = InputSystem::isKeyDown(GLFW_KEY_V);
    
    if (vKeyDown && !wasVKeyPressed)
	{
        for (VmdAnimation* animation : animations)
		{
            if (animation)
			{
                if (animation->isPlaying())
				{
                    animation->Pause();
                    std::cout << "[Animation] Paused" << std::endl;
                }
				else
				{
                    animation->Play();
                    std::cout << "[Animation] Playing" << std::endl;
                }
            }
        }
    }
    
    // 更新按键状态
    wasVKeyPressed = vKeyDown;
}
