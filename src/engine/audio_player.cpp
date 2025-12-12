#include "audio_player.h"
#include "audio.h"
#include <SDL3_mixer/SDL_mixer.h>



namespace engine {


    AudioPlayer::AudioPlayer()
    {
    }
    
    AudioPlayer::~AudioPlayer()
    {
        close();
    }

    bool AudioPlayer::init(AudioManager* audiomgr)
    {
        _audioManager = audiomgr;

        MIX_InitFlags flags = MIX_INIT_OGG | MIX_INIT_MP3;
        if ((Mix_Init(flags) & flags) != flags) 
        {
            spdlog::error("init mix failed, err = : ", SDL_GetError());
            return false;
        }

        if (!Mix_OpenAudio(0, nullptr)) 
        {
            Mix_Quit(); 
            spdlog::error("mix open audio failed: ", SDL_GetError());
            return false;
        }

        return true;
    }

    bool AudioPlayer::close()
    {
        Mix_HaltChannel(-1);

        Mix_HaltMusic();  

        Mix_CloseAudio();

        _current_music = "";

        return true;
    }

    int AudioPlayer::playSound(const std::string& name, int channel)
    {
        auto sound = _audioManager->getSound(name);
        if(!sound || !sound->_chunk)
        {
            spdlog::error("sound {} NOT found", name);
            return -1;
        }

        auto chunk = sound->_chunk;
        int played_channel = Mix_PlayChannel(channel, chunk, 0);  
        if(played_channel == -1)
        {
            spdlog::error("can NOT play sound {}", name);
        }

        return played_channel;
    }

    void AudioPlayer::setSoundVolume(float volume, int channel)
    {
        // 将浮点音量(0-1)转换为SDL_mixer的音量(0-128)
        int sdl_volume = static_cast<int>(std::max(0.0f, std::min(1.0f, volume)) * MIX_MAX_VOLUME);
        Mix_Volume(channel, sdl_volume);
    }

    float AudioPlayer::getSoundVolume(int channel)
    {
        // SDL_mixer的音量(0-128)转为 0～1.0 的浮点数
        return static_cast<float>(Mix_Volume(channel, -1)) / static_cast<float>(MIX_MAX_VOLUME);
    }

    bool AudioPlayer::playMusic(const std::string& name, int loops, int fade_in_ms)
    {
        if(name == _current_music)
        {
            return true;
        }   

        auto fmusic = _audioManager->getMusic(name);
        if(!fmusic || !fmusic->_music)
        {
            spdlog::error("music {} NOT found.", name);
            return false;
        }

        auto music = fmusic->_music;
        Mix_HaltMusic(); 

        bool result = false;
        if (fade_in_ms > 0) {
            result = Mix_FadeInMusic(music, loops, fade_in_ms);    // 淡入播放音乐
        } else {
            result = Mix_PlayMusic(music, loops);
        }

        if(!result)
        {
            spdlog::error("playe music {} failed, err = {}", name, SDL_GetError());
        }
        else 
        {
            _current_music = name;
        }

        return true;
    }

    void AudioPlayer::stopMusic(int fade_out_ms)
    {
        if (fade_out_ms > 0) {
            Mix_FadeOutMusic(fade_out_ms);  // 淡出音乐
        } else {
            Mix_HaltMusic();
        }
    }

    void AudioPlayer::pauseMusic()
    {
        Mix_PauseMusic();
    }

    void AudioPlayer::resumeMusic()
    {
        Mix_ResumeMusic();
    }

    void AudioPlayer::setMusicVolume(float volume)
    {
        int sdl_volume = static_cast<int>(std::max(0.0f, std::min(1.0f, volume)) * MIX_MAX_VOLUME);
        Mix_VolumeMusic(sdl_volume);
    }

    float AudioPlayer::getMusicVolume()
    {
        // SDL_mixer的音量(0-128)转为 0～1.0 的浮点数
        return static_cast<float>(Mix_VolumeMusic(-1)) / static_cast<float>(MIX_MAX_VOLUME);
    }



}