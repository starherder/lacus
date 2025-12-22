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
    Sound(Mix_Chunk* chunk);
    ~Sound();

private:
    Mix_Chunk* _chunk = nullptr;
};
   
// 音乐
class Music 
{
    friend class AudioPlayer;
public:
    Music() = delete;
    Music(const Music& other) = default;
    Music(Mix_Music* music);
    ~Music();

private:
    Mix_Music* _music = nullptr;
};


// 音频管理器
class AudioManager final : public IResManager
{
    using SoundPtr = std::unique_ptr<Sound>;
    using SoundMap = std::unordered_map<IdType, SoundPtr>;

    using MusicPtr = std::unique_ptr<Music>;
    using MusicMap = std::unordered_map<IdType, MusicPtr>;

public:
    AudioManager();
    AudioManager(AudioManager&&) = delete;
    AudioManager(const AudioManager&) = delete;
    ~AudioManager();

    Sound* loadSound(const HashString& file);
    Sound* getSound(const HashString& file);
    void unloadSound(const HashString& file);

    void clearSounds();

    Music* loadMusic(const HashString& file);
    Music* getMusic(const HashString& file);
    void unloadMusic(const HashString& file);

    void clearMusics();

private:
    Sound* loadSound(IdType id, const std::string_view& filepath);
    Sound* getSound(IdType id, const std::string_view& filepath="");
    void unloadSound(IdType id);

    Music* loadMusic(IdType id, const std::string_view& filepath);
    Music* getMusic(IdType id, const std::string_view& filepath="");
    void unloadMusic(IdType id);

private:
    SoundMap _sounds;
    MusicMap _musics;
};







}