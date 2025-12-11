#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <SDL_mixer.h>

namespace CS230
{
    class AudioManager
    {
    public:
        static AudioManager& Instance();

        bool Init();
        void Shutdown();

        void PlayMusic(const std::filesystem::path& path, bool loop = true);
        void StopMusic();
        void PlaySFX(const std::filesystem::path& path, int loops = 0);
        void SetMuted(bool mute);
        void SetVolumeLevel(int level, int maxLevel = 5);
        int  GetVolumeLevel() const { return volumeLevel; }
        bool IsMuted() const { return muted; }

    private:
        AudioManager()  = default;
        ~AudioManager() = default;

        bool initialized = false;
        bool muted       = false;
        int  volumeLevel = 5;
        int  volumeMax   = 5;
        void applyVolumes();
        std::string currentMusicKey;

        std::unordered_map<std::string, Mix_Chunk*> sfxCache;
        std::unordered_map<std::string, Mix_Music*> musicCache;
    };
}
