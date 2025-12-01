#include "Audio.h"
#include "../System/AudioSystem.h"
#include <iostream>

Audio::Audio(Actor* owner, const std::string& audioPath)
	: Component(owner), audioPath_(audioPath)
{
	AudioSystem::Get().Register(this);
}

Audio::~Audio()
{
	AudioSystem::Get().UnRegister(this);
	Stop();
}

void Audio::OnPlaybackEnd()
{
	isPlaying_ = false;
	std::cout << "[AudioComponent] Playback ended: " << audioPath_ << std::endl;
}
void Audio::Play()
{
	if (!sound_)
	{
		sound_ = AudioSystem::Get().Load(audioPath_);
		if (!sound_) return;
	}

	ma_sound_start(sound_.get());
	isPlaying_ = true;
	std::cout << "[AudioComponent] Playing: " << audioPath_ << std::endl;
}
void Audio::Pause()
{
	if (!sound_) return;
	ma_sound_stop(sound_.get());
	isPlaying_ = false;
}

void Audio::Stop()
{
	if (!sound_) return;
	ma_sound_stop(sound_.get());
	ma_sound_seek_to_pcm_frame(sound_.get(), 0);
	isPlaying_ = false;
}

void Audio::Seek(float seconds)
{
	if (!sound_) return;
	ma_uint32 frame = static_cast<ma_uint32>(seconds * 44100); // 假设44.1kHz
	ma_sound_seek_to_pcm_frame(sound_.get(), frame);
}
bool Audio::isPlaying() const
{
	if (!sound_) return false;
	return ma_sound_is_playing(sound_.get()) == MA_TRUE;
}

float Audio::GetDuration() const
{
	if (!sound_) return 0.0f;
	ma_uint64 frames;
	ma_sound_get_length_in_pcm_frames(sound_.get(), &frames);
	return frames / 44100.0f;
}

float Audio::GetCurrentTime() const
{
	if (!sound_) return 0.0f;
	ma_uint64 frame;
	ma_sound_get_cursor_in_pcm_frames(sound_.get(), &frame);
	return frame / 44100.0f;
}

void Audio::SetVolume(float volume)
{
	if (!sound_) return;
	ma_sound_set_volume(sound_.get(), volume);
}

float Audio::GetVolume() const
{
	if (!sound_) return 0.0f;
	return ma_sound_get_volume(sound_.get());
}

void Audio::SetLooping(bool loop)
{
	if (!sound_) return;
	ma_sound_set_looping(sound_.get(), loop ? MA_TRUE : MA_FALSE);
}

bool Audio::IsLooping() const
{
	if (!sound_) return false;
	return ma_sound_is_looping(sound_.get()) == MA_TRUE;
}
