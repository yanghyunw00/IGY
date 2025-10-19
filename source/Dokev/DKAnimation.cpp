#include "DKAnimation.hpp"
#include <algorithm>

namespace Dokev
{
    Animation::Animation(std::string name, std::vector<AnimationFrame> frames, int loopFrame) : name(std::move(name)), frames(std::move(frames)), loopFrame(loopFrame)
    {
    }

    void Animation::Update(double delta_time)
    {
        if (frames.empty())
        {
            return;
        }

        justCompleted = false;
        timer += delta_time;

        const auto& currentFrame = frames[static_cast<size_t>(currentFrameIndex)];
        if (timer >= currentFrame.duration)
        {
            timer = 0.0;
            currentFrameIndex++;

            if (static_cast<size_t>(currentFrameIndex) >= frames.size())
            {
                currentFrameIndex = loopFrame;
                justCompleted     = true;
            }
        }
    }

    int Animation::GetCurrentSpriteFrame() const
    {
        if (frames.empty())
        {
            return 0;
        }
        return frames[static_cast<size_t>(currentFrameIndex)].frameIndex;
    }

    const AnimationFrame& Animation::GetCurrentFrame() const
    {
        static const AnimationFrame defaultFrame{ 0, 1.0 };
        if (frames.empty())
        {
            return defaultFrame;
        }
        return frames[static_cast<size_t>(currentFrameIndex)];
    }

    void Animation::Reset()
    {
        currentFrameIndex = 0;
        timer             = 0.0;
        justCompleted     = false;
    }

    bool Animation::IsComplete() const
    {
        return justCompleted;
    }

    double Animation::GetTotalDuration() const
    {
        double totalDuration = 0.0;
        for (const auto& frame : frames)
        {
            totalDuration += frame.duration;
        }
        return totalDuration;
    }

    double Animation::GetElapsedTime() const
    {
        double elapsedTime = 0.0;
        for (int i = 0; i < currentFrameIndex && i < static_cast<int>(frames.size()); ++i)
        {
            elapsedTime += frames[static_cast<size_t>(i)].duration;
        }
        elapsedTime += timer;
        return elapsedTime;
    }

    double Animation::GetProgress() const
    {
        const double totalDuration = GetTotalDuration();
        if (totalDuration <= 0.0)
        {
            return 0.0;
        }
        return std::min(GetElapsedTime() / totalDuration, 1.0);
    }

    const std::string& Animation::GetName() const
    {
        return name;
    }

    size_t Animation::GetFrameCount() const
    {
        return frames.size();
    }

    int Animation::GetCurrentFrameIndex() const
    {
        return currentFrameIndex;
    }
}
