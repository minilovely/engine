#pragma once
#include "Component.h"
#include "Actor.h"
#include "../miniaudio.h"

#include <string>
#include <memory>

class Audio : public Component
{
public:
	explicit Audio(Actor* owner, const std::string& audioPath);
	~Audio();
	// 控制接口
	void Play();
	void Pause();
	void Stop();
	void Seek(float seconds);

	// 状态查询
	bool isPlaying() const;
	float GetDuration() const;
	float GetCurrentTime() const;

	// 音量控制
	void SetVolume(float volume);
	float GetVolume() const;

	ma_sound* GetSound() { return sound_.get(); }
	// 循环模式
	void SetLooping(bool loop);
	bool IsLooping() const;
private:
	std::string audioPath_;
	std::shared_ptr<ma_sound> sound_;
	bool isPlaying_ = false;

	void OnPlaybackEnd();
};

