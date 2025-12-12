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
    }

    AudioManager::~AudioManager()
    {
        clearSounds();

        clearMusics();

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


}