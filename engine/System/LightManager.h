#pragma once
#include<vector>
#include "../Scene/Light.h"
#include "../Render/Shader.h"
#include "../Scene/Actor.h"

#define MAX_LIGHT 8

class LightManager
{
public:
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;
	
	static LightManager& Get();

	void registerLight(Actor* owner, Light* light);
	void unRegisterLight(Light* light);

	std::vector<Light*> getActiveLight() const { return lights_active; }
	size_t getLightCount() const { return lights_active.size(); }

	//void updateTransform();
	void upLoadToShader(Shader* shader);
private:
	struct LightRecord
	{
		Actor* owner;
		Light* light;
	};
	std::vector<LightRecord> lights_registered;
	std::vector<Light*> lights_active;
	static LightManager* instance;
	LightManager() = default;
};

