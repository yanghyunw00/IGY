/**
 * \file DKMetaProgramming.hpp
 * \brief Compile-time meta programming utilities for performance optimization
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "Game/Common/BuildingType.hpp"
#include <cstddef>
#include <type_traits>

namespace Dokev
{
    namespace Meta
    {
        // ============================================================
        // Compile-time grid/tile calculations
        // ============================================================

        constexpr size_t tiles_needed(size_t total_size, size_t tile_size)
        {
            return (total_size + tile_size - 1) / tile_size;
        }

        constexpr size_t grid_to_index(size_t x, size_t y, size_t width)
        {
            return y * width + x;
        }

        constexpr bool in_bounds(size_t x, size_t y, size_t width, size_t height)
        {
            return x < width && y < height;
        }

        // ============================================================
        // Compile-time type traits for enums
        // ============================================================

        constexpr bool is_workshop_type(BuildingType type)
        {
            return type == BuildingType::ROCK_WORKSHOP ||
                   type == BuildingType::WOOD_WORKSHOP ||
                   type == BuildingType::STEEL_WORKSHOP;
        }

        constexpr bool is_residential_type(BuildingType type)
        {
            return type == BuildingType::HOME;
        }

        constexpr bool is_road_type(BuildingType type)
        {
            return type == BuildingType::ROAD;
        }

        // ============================================================
        // Compile-time configuration checks
        // ============================================================

        constexpr bool is_debug_mode()
        {
#ifdef _DEBUG
            return true;
#else
            return false;
#endif
        }

        constexpr bool is_release_mode()
        {
            return !is_debug_mode();
        }

        // ============================================================
        // Compile-time min/max
        // ============================================================

        template <typename T>
        constexpr T min(T a, T b)
        {
            return a < b ? a : b;
        }

        template <typename T>
        constexpr T max(T a, T b)
        {
            return a > b ? a : b;
        }

        template <typename T>
        constexpr T clamp(T val, T min_val, T max_val)
        {
            return min(max(val, min_val), max_val);
        }

        // ============================================================
        // Compile-time math
        // ============================================================

        constexpr int abs(int x)
        {
            return x < 0 ? -x : x;
        }

        constexpr float abs(float x)
        {
            return x < 0.0f ? -x : x;
        }

        constexpr double abs(double x)
        {
            return x < 0.0 ? -x : x;
        }
    }
} // namespace Dokev
