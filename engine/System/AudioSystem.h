#pragma once
#include "../miniaudio.h"
#include "../Scene/Audio.h"

#include <memory>
#include <string>
#include <unordered_map>
class Camera;

class AudioSystem
{
public:
	static AudioSystem& Get();
	void Register(Audio* audio);
	void UnRegister(Audio* audio);

	bool Init();
	void Shutdown();
	std::shared_ptr<ma_sound> Load(const std::string& filePath);

	void setMasterVolume(float volume);
	float getMasterVolume() const;

	void UpdateFromInput(float dt);
	void SetMaxDistance(float distance) { maxDistance = distance; }
private:
	AudioSystem() = default;
	~AudioSystem() { Shutdown(); }

	ma_engine engine{};
	ma_engine_config engineConfig{};
	bool initialized = false;

	bool wasKeyPressed = false;
	float masterVolume = 0.6f;
	float maxDistance = 15.0f;

	std::vector<Audio*> audios;
	std::unordered_map<std::string, std::weak_ptr<ma_sound>> soundCache;

	void UpdateSpatialAudio();
	void UpdateSpatialAudio(Camera* camera);
};
