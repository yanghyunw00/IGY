#pragma once

#include "Dokev/DKParticleEmitter.hpp"
#include "Engine/Path.hpp"

namespace Dokev
{
    class Smoke : public Dokev::AnimatedParticle {
    public:
        Smoke() : AnimatedParticle(assets::locate_asset("Assets/images/Smoke.spt")) { };

        std::string TypeName() override { return "Smoke Particle"; }
        static constexpr int MaxCount = 50;
        static constexpr double MaxLife = 10.0;
    };
}
