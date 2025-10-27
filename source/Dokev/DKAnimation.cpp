/**
 * \file DKAnimation.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKAnimation.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

namespace Dokev
{
    // ===== Animation.cpp =====

    Animation::Animation(std::string name, std::vector<AnimationFrame> frames, int loopFrame) : name(std::move(name)), frames(std::move(frames)), loopStartFrame(loopFrame), loopEndFrame(-1)
    {
    }

    Animation::Animation(const std::filesystem::path& animation_file)
    {
        std::ifstream file(animation_file);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to load: " + animation_file.string());
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            if (line.find("PlayFrame") != std::string::npos)
            {
                std::istringstream iss(line);
                std::string command;
                int    frameIndex;
                double duration;
                iss >> command >> frameIndex >> duration;
                frames.push_back({ frameIndex, duration });
            }
            else if (line.find("Loop") != std::string::npos)
            {
                std::istringstream iss(line);
                std::string command;
                int loopIndex;
                iss >> command >> loopIndex;
                loopStartFrame = loopIndex;
                loopEnabled    = true;
            }
            else if (line.find("End") != std::string::npos)
            {
                loopEnabled = false;
            }
        }
    }

    void Animation::Update(double delta_time)
    {
        if (justCompleted)
        {
            justCompleted = false;
        }

        if (frames.empty() || currentFrameIndex < 0 || currentFrameIndex >= static_cast<int>(frames.size()))
        {
            return;
        }

        const auto& currentFrame = frames[currentFrameIndex];

        if (timer == 0.0)
        {
            for (auto& event : events)
            {
                if (event.frameIndex == currentFrameIndex)
                {
                    event.callback();
                }
            }
        }

        timer += delta_time * playbackSpeed;

        if (timer >= currentFrame.duration)
        {
            timer = 0.0;
            currentFrameIndex++;

            int endFrame = (loopEndFrame == -1) ? static_cast<int>(frames.size()) : loopEndFrame;

            if (currentFrameIndex >= endFrame)
            {
                if (loopEnabled)
                {
                    currentFrameIndex = loopStartFrame;
                }
                else
                {
                    currentFrameIndex = static_cast<int>(frames.size()) - 1;
                    justCompleted     = true;
                }
            }
        }
    }

    void Animation::AddEvent(int frameIndex, std::function<void()> callback)
    {
        events.push_back({ frameIndex, std::move(callback) });
    }

    void Animation::SetLoopEnabled(bool enabled)
    {
        loopEnabled = enabled;
    }

    void Animation::SetLoopRange(int startFrame, int endFrame)
    {
        loopStartFrame = startFrame;
        loopEndFrame   = endFrame;
    }

    void Animation::SetPlaybackSpeed(double speed)
    {
        playbackSpeed = speed;
    }

    int Animation::GetCurrentSpriteFrame() const
    {
        if (frames.empty() || currentFrameIndex < 0 || currentFrameIndex >= static_cast<int>(frames.size()))
        {
            return 0;
        }
        return frames[currentFrameIndex].frameIndex;
    }

    void Animation::Reset()
    {
        currentFrameIndex = 0;
        timer = 0.0;
        justCompleted = false;
    }

    bool Animation::IsComplete() const
    {
        return justCompleted;
    }
}
