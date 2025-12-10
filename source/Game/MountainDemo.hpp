/**
 * \file MountainDemo.hpp
 * \brief Mountain-heavy terrain demo
 */
#pragma once

#include "DEMO.hpp"

class MountainDemo : public DEMO
{
public:
    void          Load() override;
    gsl::czstring GetName() const override { return "MountainDemo"; }
};
