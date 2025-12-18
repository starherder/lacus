#pragma once

#include "audio_player.h"
#include "resource.h"


namespace engine {

    class AudioManager;

    class AudioPlayer final 
    {
    public:    
        AudioPlayer();
        AudioPlayer(const AudioPlayer&) = delete;
        AudioPlayer(AudioPlayer&&) = delete;
        ~AudioPlayer();

        bool init(AudioManager* audiomgr);
        bool close();

        int playSound(const HashString& name, int channel = -1);
        void setSoundVolume(float volume, int channel = -1);
        float getSoundVolume(int channel = -1);

        bool playMusic(const HashString& name, int loops = -1, int fade_in_ms = 0);
        void stopMusic(int fade_out_ms = 0);

        void pauseMusic();
        void resumeMusic();
        
        void setMusicVolume(float volume);
        float getMusicVolume();

    private:
        AudioManager* _audioManager = nullptr;  
        HashString _current_music;
    };


}