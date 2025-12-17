#pragma once

#include "misc.h"
#include "wrapper.h"

struct Mix_Chunk;
struct Mix_Music;

namespace engine {

    
// 音效
class Sound 
{
    friend class AudioPlayer;
public:
    Sound() = delete;
    Sound(const Sound& other) = default;
    Sound(const std::string& name, Mix_Chunk* chunk);
    ~Sound();

private:
    Mix_Chunk* _chunk = nullptr;
    std::string _name;
};
   
// 音乐
class Music 
{
    friend class AudioPlayer;
public:
    Music() = delete;
    Music(const Music& other) = default;
    Music(const std::string& name, Mix_Music* music);
    ~Music();

private:
    Mix_Music* _music = nullptr;
    std::string _name;
};


// 音频管理器
class AudioManager final : public IResManager
{
    using SoundPtr = std::unique_ptr<Sound>;
    using SoundMap = std::unordered_map<std::string, SoundPtr>;

    using MusicPtr = std::unique_ptr<Music>;
    using MusicMap = std::unordered_map<std::string, MusicPtr>;

public:
    AudioManager();
    AudioManager(AudioManager&&) = delete;
    AudioManager(const AudioManager&) = delete;
    ~AudioManager();

    Sound* loadSound(const std::string& name, const std::string& filepath);
    Sound* getSound(const std::string& name, const std::string& filepath="");

    void unloadSound(const std::string& name, int size);
    void clearSounds();

    Music* loadMusic(const std::string& name, const std::string& filepath);
    Music* getMusic(const std::string& name, const std::string& filepath="");

    void unloadMusic(const std::string& name, int size);
    void clearMusics();

private:
    SoundMap _sounds;
    MusicMap _musics;

};







}