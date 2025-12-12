/**
 * \file PlainsDemo.hpp
 * \brief Plains-heavy terrain demo
 */
#pragma once

#include "DEMO.hpp"

class PlainsDemo : public DEMO
{
public:
    void Load() override;

    gsl::czstring GetName() const override
    {
        return "PlainsDemo";
    }
};
