#include "audio.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace engine {

    
    Sound::Sound(Mix_Chunk* chunk)
    {
        _chunk = chunk;
    }

    Sound::~Sound()
    {
        if (_chunk) 
        {
            Mix_FreeChunk(_chunk);
        }
    }

    Music::Music(Mix_Music* music)
    {
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
    }

    AudioManager::~AudioManager()
    {
        clearSounds();

        clearMusics();

        Mix_Quit();
    }

    Sound* AudioManager::loadSound(IdType id, const std::string_view& filepath)
    {
        auto it = _sounds.find(id);
        if (it != _sounds.end()) {
            spdlog::warn("sound {} already loaded", id);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("sound {}, path({}) NOT exist.", id, filepath);
            return nullptr;
        }
        
        auto chunk = Mix_LoadWAV(path.string().c_str());
        if (!chunk) {
            spdlog::error("Failed to load sound {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _sounds.insert({id, std::make_unique<Sound>(chunk)});
        return res ? iter->second.get() : nullptr;
    }

    Sound* AudioManager::getSound(IdType id, const std::string_view& filepath)
    {
        auto it = _sounds.find(id);
        if (it != _sounds.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return loadSound(id, filepath);
        }

        return nullptr;
    }

    void AudioManager::unloadSound(IdType id)
    {
        _sounds.erase(id);
    }

    Sound* AudioManager::loadSound(const HashString& file)
    {
        return loadSound(file.value(), file.data());
    }

    Sound* AudioManager::getSound(const HashString& file)
    {
        return getSound(file.value(), file.data());
    }

    void AudioManager::unloadSound(const HashString& file)
    {
        unloadSound(file.value());
    }

    void AudioManager::clearSounds()
    {
        _sounds.clear();
    }

    Music* AudioManager::loadMusic(IdType id, const std::string_view& filepath)
    {
        auto it = _musics.find(id);
        if (it != _musics.end()) {
            spdlog::warn("music {} already loaded", id);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("music {}, path({}) NOT exist.", id, filepath);
            return nullptr;
        }
        
        auto music = Mix_LoadMUS(path.string().c_str());
        if (!music) {
            spdlog::error("Failed to load music {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _musics.insert({id, std::make_unique<Music>(music)});
        return res ? iter->second.get() : nullptr;
    }

    Music* AudioManager::getMusic(IdType id, const std::string_view& filepath)
    {
        auto it = _musics.find(id);
        if (it != _musics.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return loadMusic(id, filepath);
        }

        return nullptr;
    }

    void AudioManager::unloadMusic(IdType id)
    {
        _musics.erase(id);
    }

    Music* AudioManager::loadMusic(const HashString& file)
    {
        return loadMusic(file.value(), file.data());
    }
    
    Music* AudioManager::getMusic(const HashString& file)
    {
        return getMusic(file.value(), file.data());
    }

    void AudioManager::unloadMusic(const HashString& file)
    {
        unloadMusic(file.value());
    }

    void AudioManager::clearMusics()
    {
        _musics.clear();
    }


}