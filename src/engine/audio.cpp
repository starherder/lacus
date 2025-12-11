#include "audio.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace engine {

    
    Sound::Sound(const std::string& name, Mix_Chunk* chunk)
    {
        _name = name;
        _chunk = chunk;
    }

    Sound::~Sound()
    {
        if (_chunk) 
        {
            Mix_FreeChunk(_chunk);
        }
    }

    Music::Music(const std::string& name, Mix_Music* music)
    {
        _name = name;
        _music = music;
    }

    Music::~Music()
    {
        if (_music) {
            Mix_FreeMusic(_music);
        }
    }

    AudioManager::AudioManager()
    {
        MIX_InitFlags flags = MIX_INIT_OGG | MIX_INIT_MP3;
        if ((Mix_Init(flags) & flags) != flags) 
        {
            spdlog::error("init mix failed, err = : ", SDL_GetError());
            assert("init mix failed");
        }

        if (!Mix_OpenAudio(0, nullptr)) {
            Mix_Quit(); 

            spdlog::error("mix open audio failed: ", SDL_GetError());
            assert("open audio failed.");
        }
    }

    AudioManager::~AudioManager()
    {
        Mix_HaltChannel(-1);

        Mix_HaltMusic();  

        clearSounds();

        clearMusics();

        Mix_CloseAudio();

        Mix_Quit();
    }

    Sound* AudioManager::loadSound(const std::string& name, const std::string& filepath)
    {
        auto it = _sounds.find(name);
        if (it != _sounds.end()) {
            spdlog::warn("sound {} already loaded", name);
            return it->second.get();
        }

        auto path = fs::current_path() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("sound {}, path({}) NOT exist.", name, filepath);
            return nullptr;
        }
        
        auto chunk = Mix_LoadWAV(path.string().c_str());
        if (!chunk) {
            spdlog::error("Failed to load sound {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _sounds.insert({name, std::make_unique<Sound>(name, chunk)});
        return res ? iter->second.get() : nullptr;
    }

    Sound* AudioManager::getSound(const std::string& name, const std::string& filepath)
    {
        auto it = _sounds.find(name);
        if (it != _sounds.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return loadSound(name, filepath);
        }

        return nullptr;
    }

    void AudioManager::unloadSound(const std::string& name, int size)
    {
        _sounds.erase(name);
    }

    void AudioManager::clearSounds()
    {
        _sounds.clear();
    }

    Music* AudioManager::loadMusic(const std::string& name, const std::string& filepath)
    {
        auto it = _musics.find(name);
        if (it != _musics.end()) {
            spdlog::warn("music {} already loaded", name);
            return it->second.get();
        }

        auto path = fs::current_path() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("music {}, path({}) NOT exist.", name, filepath);
            return nullptr;
        }
        
        auto music = Mix_LoadMUS(path.string().c_str());
        if (!music) {
            spdlog::error("Failed to load music {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _musics.insert({name, std::make_unique<Music>(name, music)});
        return res ? iter->second.get() : nullptr;
    }

    Music* AudioManager::getMusic(const std::string& name, const std::string& filepath)
    {
        auto it = _musics.find(name);
        if (it != _musics.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return loadMusic(name, filepath);
        }

        return nullptr;
    }

    void AudioManager::unloadMusic(const std::string& name, int size)
    {
        _musics.erase(name);
    }

    void AudioManager::clearMusics()
    {
        _musics.clear();
    }

    int AudioManager::playSound(const std::string& name, int channel)
    {
        auto sound = getSound(name);
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

    void AudioManager::setSoundVolume(float volume, int channel)
    {
        // 将浮点音量(0-1)转换为SDL_mixer的音量(0-128)
        int sdl_volume = static_cast<int>(std::max(0.0f, std::min(1.0f, volume)) * MIX_MAX_VOLUME);
        Mix_Volume(channel, sdl_volume);
    }

    float AudioManager::getSoundVolume(int channel)
    {
        // SDL_mixer的音量(0-128)转为 0～1.0 的浮点数
        return static_cast<float>(Mix_Volume(channel, -1)) / static_cast<float>(MIX_MAX_VOLUME);
    }

    bool AudioManager::playMusic(const std::string& name, int loops, int fade_in_ms)
    {
        if(name == _current_music)
        {
            return true;
        }   

        auto fmusic = getMusic(name);
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

    void AudioManager::stopMusic(int fade_out_ms)
    {
        if (fade_out_ms > 0) {
            Mix_FadeOutMusic(fade_out_ms);  // 淡出音乐
        } else {
            Mix_HaltMusic();
        }
    }

    void AudioManager::pauseMusic()
    {
        Mix_PauseMusic();
    }

    void AudioManager::resumeMusic()
    {
        Mix_ResumeMusic();
    }

    void AudioManager::setMusicVolume(float volume)
    {
        int sdl_volume = static_cast<int>(std::max(0.0f, std::min(1.0f, volume)) * MIX_MAX_VOLUME);
        Mix_VolumeMusic(sdl_volume);
    }

    float AudioManager::getMusicVolume()
    {
        // SDL_mixer的音量(0-128)转为 0～1.0 的浮点数
        return static_cast<float>(Mix_VolumeMusic(-1)) / static_cast<float>(MIX_MAX_VOLUME);
    }

}