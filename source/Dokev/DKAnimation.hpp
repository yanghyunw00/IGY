#pragma once

#include "Engine/Vec2.hpp"
#include <string>
#include <vector>

namespace Dokev
{

    struct AnimationFrame
    {
        int    frameIndex;  
        double duration;  
    };


    class Animation
    {
    public:
        Animation() = default;
        Animation(std::string name, std::vector<AnimationFrame> frames, int loopFrame = 0);

        void Update(double delta_time);

        int GetCurrentSpriteFrame() const;

        const AnimationFrame& GetCurrentFrame() const;

        void Reset();

        bool IsComplete() const;

        double GetTotalDuration() const;

        double GetElapsedTime() const;

        double GetProgress() const;

        const std::string& GetName() const;

        size_t GetFrameCount() const;

        int GetCurrentFrameIndex() const;

    private:
        std::string                  name;
        std::vector<AnimationFrame>  frames;
        int                          loopFrame{ 0 };
        int                          currentFrameIndex{ 0 };
        double                       timer{ 0.0 };
        bool                         justCompleted{ false };
    };
}
