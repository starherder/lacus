#include "wrapper.h"

struct Mix_Chunk;
struct Mix_Music;

namespace engine {

    
// 音效
class Sound 
{
    friend class AudioManager;
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
    friend class AudioManager;
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
class AudioManager 
{
    using SoundPtr = std::unique_ptr<Sound>;
    using SoundMap = std::unordered_map<std::string, SoundPtr>;

    using MusicPtr = std::unique_ptr<Music>;
    using MusicMap = std::unordered_map<std::string, MusicPtr>;

public:
    AudioManager();
    ~AudioManager();

    Sound* loadSound(const std::string& name, const std::string& filepath);
    Sound* getSound(const std::string& name, const std::string& filepath="");

    void unloadSound(const std::string& name, int size);
    void clearSounds();

    Music* loadMusic(const std::string& name, const std::string& filepath);
    Music* getMusic(const std::string& name, const std::string& filepath="");

    void unloadMusic(const std::string& name, int size);
    void clearMusics();

    int playSound(const std::string& name, int channel = -1);
    void setSoundVolume(float volume, int channel = -1);
    float getSoundVolume(int channel = -1);

    bool playMusic(const std::string& name, int loops = -1, int fade_in_ms = 0);
    void stopMusic(int fade_out_ms = 0);
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume);
    float getMusicVolume();

private:
    SoundMap _sounds;
    MusicMap _musics;

    std::string _current_music;
};







}