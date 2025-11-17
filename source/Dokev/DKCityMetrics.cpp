#include "DKCityMetrics.hpp"

namespace Dokev
{
    void CityMetrics::Update(double dt)
    {
        averageHappiness += happinessChangeRate * 0.3f * static_cast<float>(dt);

        if (averageHappiness < 0.0f)
            averageHappiness = 0.0f;
        if (averageHappiness > 100.0f)
            averageHappiness = 100.0f;
    }
}