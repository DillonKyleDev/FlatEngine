#pragma once
#include <string>
#include <SDL_mixer.h>


namespace FlatEngine
{
	class Sound
	{
	public:
		Sound();
		~Sound();

		Mix_Music* LoadMusic(std::string path);
		Mix_Chunk* LoadEffect(std::string path);
		int GetMusicVolume();
		void SetMusicVolume(int volume);
		int GetEffectVolume();
		void SetEffectVolume(int volume);
		void PlayMusic();
		void PauseMusic();
		bool IsPaused();
		void StopMusic();
		bool IsMusicPlaying();
		void PlayEffect();
		void HaultChannel();
		bool Loops();
		void SetLoops(bool b_loops);

	private:
		Mix_Music* m_music;
		Mix_Chunk* m_effect;
		std::string m_path;
		int m_effectVolume;
		int m_effectChannel;
		int m_musicVolume;
		bool m_b_isPaused;
		bool m_b_loopMusic;
	};
}
