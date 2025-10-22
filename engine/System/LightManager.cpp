#include "LightManager.h"
#include "../Scene/Transform.h"

#include <iostream>

LightManager& LightManager::Get()
{
	static LightManager instance;
	return instance;
}

void LightManager::registerLight(Actor* owner, Light* light)
{
	auto it = std::find_if(lights_registered.begin(), lights_registered.end(),
		[light](const LightRecord& record) {return record.light == light; });

	if (it == lights_registered.end())
	{
		lights_registered.push_back({ owner,light });
		lights_active.push_back(light);
		std::cout << "[LightManager] Registered light, total: " 
			<< lights_registered.size() << std::endl;

	}
}

void LightManager::unRegisterLight(Light* light)
{
	lights_registered.erase(std::remove_if(lights_registered.begin(), lights_registered.end(),
		[light](const LightRecord& record) {return record.light == light; }));
	lights_active.erase(
		std::remove(lights_active.begin(), lights_active.end(), light),lights_active.end());

}

void LightManager::upLoadToShader(Shader* shader)
{
	if (!shader)
	{
		std::cout << "[Light Manager] No shader exist!" << std::endl;
		return;
	}
	//updateTransform();
	size_t lightCount = std::min(lights_active.size(), size_t(MAX_LIGHT)); // 限制最大光源数

	shader->setInt("lightCount", lightCount);

	for (size_t i = 0; i < lights_active.size(); i++)
	{
		lights_active[i]->UploadToShader(shader,i);
	}
}

