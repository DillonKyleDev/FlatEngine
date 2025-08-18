#include "Sound.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Sound::Sound()
	{
		m_path = "";
		m_b_isPaused = false;
		m_music = nullptr;
		m_effect = nullptr;
		m_musicVolume = F_musicVolume;
		m_effectVolume = F_effectVolume;
		m_effectChannel = 0;
		m_b_loopMusic = false;
		Mix_Volume(-1, m_effectVolume);
		Mix_VolumeMusic(m_musicVolume);
	}
	Sound::~Sound()
	{
		Mix_FreeMusic(m_music);
		Mix_FreeChunk(m_effect);
		m_music = nullptr;
		m_effect = nullptr;
	}

	int Sound::GetMusicVolume()
	{
		return Mix_VolumeMusic(-1);
	}

	// Max volume is 128
	void Sound::SetMusicVolume(int volume)
	{
		Mix_VolumeMusic(volume);
	}

	int Sound::GetEffectVolume()
	{
		return Mix_Volume(-1, -1);
	}

	// Max volume is 128
	void Sound::SetEffectVolume(int volume)
	{
		m_effectVolume = volume;
	}

	Mix_Music* Sound::LoadMusic(std::string path)
	{
		m_music = Mix_LoadMUS(path.c_str());
		return m_music;
	}

	Mix_Chunk* Sound::LoadEffect(std::string path)
	{
		m_effect = Mix_LoadWAV(path.c_str());
		return m_effect;
	}

	void Sound::PlayMusic()
	{
		SetMusicVolume(m_musicVolume);

		if (Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(m_music, m_b_loopMusic);
		}
		else
		{
			if (Mix_PausedMusic() == 1)
			{
				Mix_ResumeMusic();
			}
			else
			{
				Mix_PauseMusic();
			}
		}
	}

	void Sound::PauseMusic()
	{
		if (Mix_PausedMusic() == 1)
		{
			Mix_ResumeMusic();
		}
		else
		{
			Mix_PauseMusic();
		}
	}

	bool Sound::IsPaused()
	{
		return Mix_PausedMusic() == 1;
	}

	void Sound::StopMusic()
	{
		Mix_HaltMusic();
	}

	bool Sound::IsMusicPlaying()
	{
		return Mix_PlayingMusic();
	}

	void Sound::PlayEffect()
	{
		// Play effect on channel, repeat 0 times
		m_effectChannel = GetNextAvailableEffectChannel();		
		Mix_Volume(m_effectChannel, m_effectVolume);
		Mix_PlayChannel(m_effectChannel, m_effect, 0);
	}

	void Sound::HaultChannel()
	{
		Mix_HaltChannel(m_effectChannel);
	}

	bool Sound::Loops()
	{
		return m_b_loopMusic;
	}

	void Sound::SetLoops(bool b_loops)
	{
		m_b_loopMusic = b_loops;
	}
}