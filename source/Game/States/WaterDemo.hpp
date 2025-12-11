/**
 * \file WaterDemo.hpp
 * \brief Water-heavy terrain demo
 */
#pragma once

#include "DEMO.hpp"

class WaterDemo : public DEMO
{
public:
    void          Load() override;
    gsl::czstring GetName() const override { return "WaterDemo"; }
};
