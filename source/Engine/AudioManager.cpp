/**
 * \file AudioManager.cpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "AudioManager.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Path.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <cctype>
#include <exception>
#include <random>

namespace
{
    std::string normalizeKey(const std::filesystem::path& path)
    {
        return path.lexically_normal().string();
    }

    bool isAudioFile(const std::filesystem::path& path)
    {
        auto extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return extension == ".mp3" || extension == ".ogg" || extension == ".wav";
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

        StopLayeredMusic();
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
        StopLayeredMusic();
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

    void AudioManager::PlayLayeredMusic(const std::vector<std::filesystem::path>& layerFolders)
    {
        if (!initialized && !Init())
        {
            return;
        }

        StopMusic();
        StopLayeredMusic();

        std::vector<LayeredMusicLayer> newLayers;
        newLayers.reserve(layerFolders.size());

        for (const auto& folder : layerFolders)
        {
            std::filesystem::path folderPath;
            try
            {
                folderPath = assets::locate_asset(folder);
            }
            catch (const std::exception& e)
            {
                Engine::GetLogger().LogError(std::string("Layered music folder not found: ") + e.what());
                continue;
            }

            if (!std::filesystem::is_directory(folderPath))
            {
                Engine::GetLogger().LogError("Layered music path is not a folder: " + folderPath.string());
                continue;
            }

            LayeredMusicLayer layer;
            layer.name = folderPath.filename().string();

            for (const auto& entry : std::filesystem::directory_iterator(folderPath))
            {
                if (entry.is_regular_file() && isAudioFile(entry.path()))
                {
                    layer.playlist.push_back(entry.path());
                }
            }

            std::sort(layer.playlist.begin(), layer.playlist.end());
            if (!layer.playlist.empty())
            {
                newLayers.push_back(std::move(layer));
            }
            else
            {
                Engine::GetLogger().LogError("No audio files found for layered music: " + folderPath.string());
            }
        }

        std::random_device randomDevice;
        std::mt19937       rng(randomDevice());
        std::shuffle(newLayers.begin(), newLayers.end(), rng);
        for (auto& layer : newLayers)
        {
            std::shuffle(layer.playlist.begin(), layer.playlist.end(), rng);
        }

        layeredMusic = std::move(newLayers);
        Mix_ReserveChannels(static_cast<int>(layeredMusic.size()));

        if (!muted && volumeLevel > 0)
        {
            UpdateLayeredMusic();
        }
    }

    void AudioManager::UpdateLayeredMusic()
    {
        if (!initialized && !Init())
        {
            return;
        }

        if (muted || volumeLevel == 0)
        {
            haltLayeredMusicChannels();
            return;
        }

        for (std::size_t i = 0; i < layeredMusic.size(); ++i)
        {
            auto& layer = layeredMusic[i];
            if (layer.playlist.empty())
            {
                continue;
            }

            if (layer.channel >= 0 && Mix_Playing(layer.channel))
            {
                continue;
            }

            if (layer.nextTrack >= layer.playlist.size())
            {
                std::random_device randomDevice;
                std::mt19937       rng(randomDevice());
                std::shuffle(layer.playlist.begin(), layer.playlist.end(), rng);
                layer.nextTrack = 0;
            }

            const auto trackPath = layer.playlist[layer.nextTrack++];
            const auto key       = normalizeKey(trackPath);

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
                    Engine::GetLogger().LogError(std::string("Failed to load layered music: ") + key + " - " + Mix_GetError());
                    layer.currentTrack.clear();
                    layer.channel = -1;
                    continue;
                }
                sfxCache[key] = chunk;
            }

            const int channel = static_cast<int>(i);
            if (Mix_PlayChannel(channel, chunk, 0) == -1)
            {
                Engine::GetLogger().LogError(std::string("Failed to play layered music: ") + key + " - " + Mix_GetError());
                layer.currentTrack.clear();
                layer.channel = -1;
                continue;
            }

            layer.channel      = channel;
            layer.currentTrack = trackPath.filename().string();
        }
    }

    void AudioManager::StopLayeredMusic()
    {
        haltLayeredMusicChannels();
        Mix_ReserveChannels(0);
        layeredMusic.clear();
    }

    std::vector<std::string> AudioManager::GetLayeredMusicDebugText() const
    {
        std::vector<std::string> debugText;
        debugText.reserve(layeredMusic.size());

        for (const auto& layer : layeredMusic)
        {
            const std::string trackName = layer.currentTrack.empty() ? "(stopped)" : layer.currentTrack;
            debugText.push_back(layer.name + ": " + trackName);
        }

        return debugText;
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
            haltLayeredMusicChannels();
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
            haltLayeredMusicChannels();
            Mix_HaltChannel(-1);
        }
    }

    void AudioManager::applyVolumes()
    {
        const int scaled = (muted || volumeLevel == 0) ? 0 : static_cast<int>(std::lround(static_cast<double>(volumeLevel) * MIX_MAX_VOLUME / volumeMax));
        Mix_VolumeMusic(scaled);
        Mix_Volume(-1, scaled);
    }

    void AudioManager::haltLayeredMusicChannels()
    {
        for (auto& layer : layeredMusic)
        {
            if (layer.channel >= 0)
            {
                Mix_HaltChannel(layer.channel);
            }
            layer.channel = -1;
            layer.currentTrack.clear();
        }
    }

}
