#define MINIAUDIO_IMPLEMENTATION
#include "AudioSystem.h"
#include "InputSystem.h"
#include "CameraSystem.h"
#include "../Scene/Camera.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include "../Scene/Transform.h"

AudioSystem& AudioSystem::Get()
{
	static AudioSystem instance;
	return instance;
}
void AudioSystem::Register(Audio* audio)
{
	audios.push_back(audio);
}
void AudioSystem::UnRegister(Audio* audio)
{
	audios.erase(std::remove(audios.begin(), audios.end(), audio), audios.end());
}
bool AudioSystem::Init()
{
	if (initialized) return true;
	engineConfig = ma_engine_config_init();
	engineConfig.channels = 2;
	engineConfig.sampleRate = 44100;
	if (ma_engine_init(&engineConfig, &engine) != MA_SUCCESS)
	{
		std::cerr << "[AudioSystem] Failed to initialize audio engine!" << std::endl;
		return false;
	}
	// 设置默认主音量
	ma_engine_set_volume(&engine, masterVolume);
	initialized = true;
	std::cout << "[AudioSystem] Initialized successfully" << std::endl;
	return true;
}
void AudioSystem::Shutdown()
{
	if (!initialized) return;

	ma_engine_uninit(&engine);
	initialized = false;
	soundCache.clear();
	std::cout << "[AudioSystem] Shutdown" << std::endl;
}

std::shared_ptr<ma_sound> AudioSystem::Load(const std::string& filePath)
{
	if (!initialized) return nullptr;

	// 检查缓存
	auto it = soundCache.find(filePath);
	if (it != soundCache.end() && !it->second.expired())
	{
		return it->second.lock();
	}

	// 加载新音频
	auto sound = std::make_shared<ma_sound>();
	if (ma_sound_init_from_file(&engine, filePath.c_str(), 0, nullptr, nullptr, sound.get()) != MA_SUCCESS)
	{
		std::cerr << "[AudioSystem] Failed to load: " << filePath << std::endl;
		return nullptr;
	}
	ma_sound_set_spatialization_enabled(sound.get(), MA_TRUE);
	ma_sound_set_doppler_factor(sound.get(), 1.0f);
	// 加入缓存
	soundCache[filePath] = sound;
	std::cout << "[AudioSystem] Loaded: " << filePath << std::endl;
	return sound;
}

void AudioSystem::setMasterVolume(float volume)
{
	if (!initialized) return;
	masterVolume = volume;
	ma_engine_set_volume(&engine, masterVolume);
}

float AudioSystem::getMasterVolume() const
{
	return masterVolume;
}

void AudioSystem::UpdateFromInput(float dt)
{
	bool KeyDown = InputSystem::isKeyDown(GLFW_KEY_B);

	if (KeyDown && !wasKeyPressed)
	{
		if (!audios.empty())
		{
			auto* audio = audios[0];
			if (audio->isPlaying())
			{
				audio->Pause();
				std::cout << "[Audio] Paused" << std::endl;
			}
			else
			{
				audio->Play();
				std::cout << "[Audio] Playing" << std::endl;
			}
		}
	}
	wasKeyPressed = KeyDown;

	bool upKey = InputSystem::isKeyDown(GLFW_KEY_UP);
	bool downKey = InputSystem::isKeyDown(GLFW_KEY_DOWN);

	if (upKey || downKey)
	{
		float volumeChange = upKey ? 0.02f : -0.02f;
		float newVolume = masterVolume + volumeChange;

		// 使用glm::clamp限制在0.0到1.0之间
		newVolume = glm::clamp(newVolume, 0.0f, 1.0f);

		// 仅当音量实际变化时才更新
		if (newVolume != masterVolume)
		{
			setMasterVolume(newVolume);
			ma_engine_set_volume(&engine, masterVolume);
			std::cout << "[Audio] Master Volume: " << (int)(newVolume * 100) << "%" << std::endl;
		}
	}
	UpdateSpatialAudio();
}

void AudioSystem::UpdateSpatialAudio()
{
	Camera* mainCam = CameraSystem::Get().GetMainCamera();
	if (mainCam)
	{
		UpdateSpatialAudio(mainCam);
	}
}

void AudioSystem::UpdateSpatialAudio(Camera* camera)
{
	if (!camera || audios.empty()) return;

	// 获取相机位置（从Transform组件）
	auto camTransform = camera->GetOwner()->GetComponent<Transform>();
	if (!camTransform) return;

	glm::vec3 camPos = camTransform->getPosition();
	// 设置3D听众（相机）
	ma_engine_listener_set_position(&engine, 0, camPos.x, camPos.y, camPos.z);

	// 获取相机朝向
	glm::vec3 camForward = camera->getCenter() - camPos;
	if (glm::length(camForward) > 0.001f) {
		camForward = glm::normalize(camForward);
		ma_engine_listener_set_direction(&engine, 0, camForward.x, camForward.y, camForward.z);
	}
	ma_engine_listener_set_world_up(&engine, 0, 0, 1, 0); // Y轴向上

	// 遍历所有音频组件，更新其空间参数
	for (Audio* audio : audios)
	{
		auto* trans = audio->GetOwner()->GetComponent<Transform>();
		if (!trans) continue;

		glm::vec3 audioPos = trans->getPosition();
		ma_sound_set_position(audio->GetSound(), audioPos.x, audioPos.y, audioPos.z);

		// 设置衰减范围
		ma_sound_set_min_distance(audio->GetSound(), 3.0f);
		ma_sound_set_max_distance(audio->GetSound(), maxDistance);
	}
}
