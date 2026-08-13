#include "components/Audio.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"


namespace FlatEngine
{
    Audio::Audio(long ownerID)
    {
        SetType(ComponentType_Audio);
        SetOwnerID(ownerID);
        m_sounds = std::vector<SoundData>();
    }

    json Audio::GetData(bool b_IDOverride)
    {
        json soundData = json::array();

        for (SoundData sound : m_sounds)
        {
            json soundJson = {
                { "path", sound.path },
                { "name", sound.name },
                { "b_isMusic", sound.b_isMusic }
            };
            soundData.push_back(soundJson);
        }

        json componentJson = {
            { "soundData", soundData }
        };
        componentJson.update(Component::GetData(b_IDOverride));

        return componentJson;
    }

    void Audio::PutData(json componentJson, std::string objectName)
	{
        if (componentJson.empty())		
			return;		

        Component::PutData(componentJson, objectName);

        if (JsonHelper::JsonContains(componentJson, "soundData", objectName))
        {
            for (int sound = 0; sound < componentJson.at("soundData").size(); sound++)
            {										
                json soundJson = componentJson.at("soundData").at(sound);
                std::string path = JsonHelper::CheckJsonString(soundJson, "path", objectName);
                std::string soundName = JsonHelper::CheckJsonString(soundJson, "name", objectName);
                if (!FileHelper::DoesFileExist(path))
                {
                    Logger::log.Err("Audio file not found for GameObject: {} - on Audio: {}. This may lead to unexpected behavior.  \npath: ", objectName, soundName, path);
                }
                bool b_isMusic = JsonHelper::CheckJsonBool(soundJson, "b_isMusic", objectName);

                AddSound(soundName, path);
            }
        }
    }

    void Audio::SetPath(std::string soundName, std::string newPath)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                sound.path = newPath;
                LoadAudio(sound);
            }
        }
    }

    std::vector<SoundData> &Audio::GetSounds()
    {
        return m_sounds;
    }

    std::string Audio::GetPath(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                return sound.path;
            }
        }
        return "";
    }

    bool Audio::ContainsName(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                return true;
            }
        }
        return false;
    }

    void Audio::SetIsMusic(std::string soundName, bool b_isMusic)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                sound.b_isMusic = b_isMusic;
                LoadAudio(sound);
                break;
            }
        }
    }

    void Audio::LoadAudio(SoundData &soundData)
    {
        if (soundData.path != "" && soundData.sound != nullptr)
        {
            if (FlatEngine::FileHelper::FilepathHasExtension(soundData.path, ".mp3"))
            {
                soundData.sound->LoadMusic(soundData.path);
                soundData.b_isMusic = true;
            }
            else
            {
                soundData.sound->LoadEffect(soundData.path);
                soundData.b_isMusic = false;
            }
        }
    }

    bool Audio::IsMusic(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                return sound.b_isMusic;
            }
        }
        return false;
    }

    // Sound files with .mp3 extension are created as Music, any other extension is created as a Sound
    void Audio::AddSound(std::string soundName, std::string soundPath)
    {
        SoundData soundData;
        soundData.name = soundName;
        soundData.path = soundPath;
        soundData.sound = std::make_shared<AudioManager::Sound>();

        for (std::vector<SoundData>::iterator iter = m_sounds.begin(); iter != m_sounds.end(); iter++)
        {
            if (iter->name == soundName)
            {
                m_sounds.erase(iter);
                break;
            }
        }

        LoadAudio(soundData);
        m_sounds.push_back(soundData);
    }

    void Audio::SetSounds(std::vector<SoundData> sounds)
    {
        m_sounds = sounds;
    }

    void Audio::RemoveSound(std::string soundName)
    {
        for (std::vector<SoundData>::iterator iter = m_sounds.begin(); iter != m_sounds.end(); iter++)
        {
            if (iter->name == soundName)
            {
                m_sounds.erase(iter);
                break;
            }
        }
    }

    void Audio::PlaySound(std::string soundName)
    {
        bool b_audioFound = false;
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                Play(sound.name);
                b_audioFound = true;
            }	
        }
        if (!b_audioFound)
        {
            Logger::log.Err("Audio sound {} not found.", soundName);
        }
    }

    void Audio::PauseSound(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                Pause(sound.name);
            }
        }
    }

    bool Audio::IsPaused(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                return sound.sound->IsPaused();
            }
        }
        return false;
    }

    void Audio::StopSound(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                Stop(sound.name);
            }
        }
    }

    // Max volume is 128
    void Audio::SetEffectVolume(std::string soundName, int volume)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {				
                sound.sound->SetEffectVolume(volume);
            }
        }
    }

    // Max volume is 128
    void Audio::SetMusicVolume(std::string soundName, int volume)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                sound.sound->SetMusicVolume(volume);
            }
        }
    }

    void Audio::StopAll()
    {
        for (SoundData sound : m_sounds)
        {
            Stop(sound.name);
        }
    }

    void Audio::Play(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                if (sound.b_isMusic)
                {
                    sound.sound->PlayMusic();
                }
                else
                {
                    sound.sound->PlayEffect();
                }
            }
        }
    }

    void Audio::Pause(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                sound.sound->PauseMusic();
                sound.sound->HaultChannel();
            }
        }
    }

    void Audio::Stop(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                sound.sound->StopMusic();
                sound.sound->HaultChannel();
            }
        }
    }

    bool Audio::IsMusicPlaying(std::string soundName)
    {
        for (SoundData sound : m_sounds)
        {
            if (sound.name == soundName)
            {
                return sound.sound->IsMusicPlaying();
            }
        }
        return false;
    }
}