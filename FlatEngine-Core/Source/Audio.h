#pragma once
#include "Sound.h"
#include "Component.h"

#include <string>
#include <vector>
#include <map>
#include <memory>


namespace FlatEngine
{
	struct SoundData {
		std::string name;
		std::string path;
		std::shared_ptr<Sound> sound;
		bool b_isMusic;
	};

	class Audio : public Component
	{
	public:
		Audio(long myID = -1, long parentID = -1);
		~Audio();
		std::string GetData();

		std::vector<SoundData>& GetSounds();
		std::string GetPath(std::string soundName);
		bool ContainsName(std::string soundName);
		void SetPath(std::string soundName, std::string newPath);
		void SetIsMusic(std::string soundName, bool b_isMusic);
		void LoadAudio(SoundData& soundData);
		bool IsMusic(std::string soundName);
		void AddSound(std::string soundName, std::string soundPath);
		void SetSounds(std::vector<SoundData> sounds);
		void RemoveSound(std::string soundName);
		bool IsPaused(std::string soundName);
		void StopAll();
		bool IsMusicPlaying(std::string soundName);
		void Play(std::string soundName);
		void Pause(std::string soundName);
		void Stop(std::string soundName);
		// Lua function wrappers because default parameters are not allowed in Lua
		void PlaySound(std::string soundName);
		void PauseSound(std::string soundName);
		void StopSound(std::string soundName);
		void SetEffectVolume(std::string soundName, int volume);
		void SetMusicVolume(std::string soundName, int volume);

	private:
		std::vector<SoundData> m_sounds;
	};
}
