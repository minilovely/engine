#define MINIAUDIO_IMPLEMENTATION
#include "AudioSystem.h"
#include "InputSystem.h"
#include "GLFW/glfw3.h"

#include <iostream>

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

	// 加入缓存
	soundCache[filePath] = sound;
	std::cout << "[AudioSystem] Loaded: " << filePath << std::endl;
	return sound;
}

void AudioSystem::setMasterVolume(float volume)
{
	if (!initialized) return;
	ma_engine_set_volume(&engine, volume);
}

float AudioSystem::getMasterVolume() const
{
	return ma_engine_get_volume(const_cast<ma_engine*>(&engine));
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
}
