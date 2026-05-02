/**
 * \file BuildingType.hpp
 * \brief Building type enumeration
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

enum class BuildingType
{
    NONE = 0,

    // WORK buildings
    ROCK_WORKSHOP,
    WOOD_WORKSHOP,
    STEEL_WORKSHOP,
    GRANARY,

    // RESIDENT buildings
    HOME,

    // INFRASTRUCTURE
    ROAD,
    // Delete
    DELETE,
    // Terrain
    FLATTEN
};
