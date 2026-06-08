#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

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
        void PlayLayeredMusic(const std::vector<std::filesystem::path>& layerFolders);
        void UpdateLayeredMusic();
        void StopLayeredMusic();
        std::vector<std::string> GetLayeredMusicDebugText() const;
        void PlaySFX(const std::filesystem::path& path, int loops = 0);
        void SetMuted(bool mute);
        void SetVolumeLevel(int level, int maxLevel = 5);

        int GetVolumeLevel() const
        {
            return volumeLevel;
        }

        bool IsMuted() const
        {
            return muted;
        }

    private:
        AudioManager()  = default;
        ~AudioManager() = default;

        bool        initialized = false;
        bool        muted       = false;
        int         volumeLevel = 5;
        int         volumeMax   = 5;
        void        applyVolumes();
        void        haltLayeredMusicChannels();
        std::string currentMusicKey;

        struct LayeredMusicLayer
        {
            std::string                       name;
            std::vector<std::filesystem::path> playlist;
            std::size_t                       nextTrack = 0;
            int                               channel   = -1;
            std::string                       currentTrack;
        };

        std::vector<LayeredMusicLayer> layeredMusic;

        std::unordered_map<std::string, Mix_Chunk*> sfxCache;
        std::unordered_map<std::string, Mix_Music*> musicCache;
    };
}
