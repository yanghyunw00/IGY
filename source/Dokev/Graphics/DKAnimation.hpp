/**
 * \file DKAnimation.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>


namespace Dokev
{
    struct AnimationFrame
    {
        int    frameIndex;
        double duration;
    };

    struct AnimationEvent
    {
        int                   frameIndex;
        std::function<void()> callback;
    };

    class Animation
    {
    public:
        Animation() = default;

        Animation(std::string name, std::vector<AnimationFrame> frames, int loopFrame = 0);

        Animation(const std::filesystem::path& animation_file);

        void Update(double delta_time);
        int  GetCurrentSpriteFrame() const;
        void Reset();
        bool IsComplete() const;

        void AddEvent(int frameIndex, std::function<void()> callback);

        void SetLoopEnabled(bool enabled);
        void SetLoopRange(int startFrame, int endFrame);

        void SetPlaybackSpeed(double speed);

    private:
        std::string                 name;
        std::vector<AnimationFrame> frames;
        std::vector<AnimationEvent> events;

        int    loopStartFrame{ 0 };
        int    loopEndFrame{ -1 };
        bool   loopEnabled{ true };
        double playbackSpeed{ 1.0 };

        int    currentFrameIndex{ 0 };
        double timer{ 0.0 };
        bool   justCompleted{ false };
    };
}