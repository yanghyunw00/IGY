#include "AudioManager.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Path.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <exception>

namespace
{
    std::string normalizeKey(const std::filesystem::path& path)
    {
        return path.lexically_normal().string();
    }
}

namespace CS230
{

    AudioManager& AudioManager::Instance()
    {
        static AudioManager instance;
        return instance;
    }

    bool AudioManager::Init()
    {
        if (initialized)
        {
            return true;
        }

        // Ensure audio subsystem is initialized (SDL_Init in Window only enables video)
        if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
        {
            if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
            {
                Engine::GetLogger().LogError(std::string("SDL audio init failed: ") + SDL_GetError());
                return false;
            }
        }

        const int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
        if ((Mix_Init(flags) & flags) != flags)
        {
            Engine::GetLogger().LogError(std::string("SDL_mixer init failed: ") + Mix_GetError());
            return false;
        }

        // Slightly larger buffer to avoid underruns when loading assets or switching states
        constexpr int kFrequency   = 48000;
        constexpr int kChunkSize   = 8192; // slightly larger buffer to reduce dropouts on slow frames
        constexpr int kNumChannels = 2;
        if (Mix_OpenAudio(kFrequency, MIX_DEFAULT_FORMAT, kNumChannels, kChunkSize) < 0)
        {
            Engine::GetLogger().LogError(std::string("SDL_mixer open audio failed: ") + Mix_GetError());
            Mix_Quit();
            return false;
        }

        Mix_AllocateChannels(32);
        applyVolumes();
        initialized = true;
        return true;
    }

    void AudioManager::Shutdown()
    {
        if (!initialized)
        {
            return;
        }

        StopMusic();

        for (auto& entry : sfxCache)
        {
            if (entry.second)
            {
                Mix_FreeChunk(entry.second);
            }
        }
        sfxCache.clear();

        for (auto& entry : musicCache)
        {
            if (entry.second)
            {
                Mix_FreeMusic(entry.second);
            }
        }
        musicCache.clear();

        Mix_CloseAudio();
        Mix_Quit();

        initialized = false;
    }

    void AudioManager::PlayMusic(const std::filesystem::path& path, bool loop)
    {
        if (!initialized && !Init())
        {
            return;
        }
        if (muted || volumeLevel == 0)
        {
            StopMusic();
            return;
        }

        std::filesystem::path assetPath;
        try
        {
            assetPath = assets::locate_asset(path);
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError(std::string("Audio asset not found: ") + e.what());
            return;
        }
        const auto key = normalizeKey(assetPath);

        Mix_Music* music = nullptr;
        auto       it    = musicCache.find(key);
        if (it != musicCache.end())
        {
            music = it->second;
        }
        else
        {
            music = Mix_LoadMUS(key.c_str());
            if (!music)
            {
                Engine::GetLogger().LogError(std::string("Failed to load music: ") + key + " - " + Mix_GetError());
                return;
            }
            musicCache[key] = music;
        }

        if (currentMusicKey == key && Mix_PlayingMusic())
        {
            return;
        }

        if (Mix_PlayMusic(music, loop ? -1 : 0) == -1)
        {
            Engine::GetLogger().LogError(std::string("Failed to play music: ") + key + " - " + Mix_GetError());
            return;
        }

        currentMusicKey = key;
    }

    void AudioManager::StopMusic()
    {
        Mix_HaltMusic();
        currentMusicKey.clear();
    }

    void AudioManager::PlaySFX(const std::filesystem::path& path, int loops)
    {
        if (!initialized && !Init())
        {
            return;
        }
        if (muted || volumeLevel == 0)
        {
            return;
        }

        std::filesystem::path assetPath;
        try
        {
            assetPath = assets::locate_asset(path);
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError(std::string("Audio asset not found: ") + e.what());
            return;
        }
        const auto key = normalizeKey(assetPath);

        Mix_Chunk* chunk = nullptr;
        auto       it    = sfxCache.find(key);
        if (it != sfxCache.end())
        {
            chunk = it->second;
        }
        else
        {
            chunk = Mix_LoadWAV(key.c_str());
            if (!chunk)
            {
                Engine::GetLogger().LogError(std::string("Failed to load sfx: ") + key + " - " + Mix_GetError());
                return;
            }
            sfxCache[key] = chunk;
        }

        if (Mix_PlayChannel(-1, chunk, loops) == -1)
        {
            Engine::GetLogger().LogError(std::string("Failed to play sfx: ") + key + " - " + Mix_GetError());
        }
    }

    void AudioManager::SetMuted(bool mute)
    {
        muted = mute;
        if (muted)
        {
            Mix_HaltMusic();
            Mix_HaltChannel(-1);
        }
        applyVolumes();
    }

    void AudioManager::SetVolumeLevel(int level, int maxLevel)
    {
        volumeMax   = std::max(1, maxLevel);
        volumeLevel = std::clamp(level, 0, volumeMax);
        applyVolumes();
        if (volumeLevel == 0)
        {
            Mix_HaltMusic();
            Mix_HaltChannel(-1);
        }
    }

    void AudioManager::applyVolumes()
    {
        const int scaled = (muted || volumeLevel == 0) ? 0 : static_cast<int>(std::lround(static_cast<double>(volumeLevel) * MIX_MAX_VOLUME / volumeMax));
        Mix_VolumeMusic(scaled);
        Mix_Volume(-1, scaled);
    }

}
