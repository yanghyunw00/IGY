/**
 * \file DKCompileTimeExamples.hpp
 * \brief Examples of using if constexpr for compile-time optimization
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once

#include "DKMetaProgramming.hpp"
#include "Game/BuildingType.hpp"
#include <iostream>

namespace Dokev
{
    /**
     * @brief Template function that handles different building types at compile time
     * This eliminates runtime checks for building type!
     */
    template<BuildingType Type>
    constexpr const char* GetBuildingCategory()
    {
        if constexpr (Meta::is_workshop_type(Type))
        {
            return "Workshop";
        }
        else if constexpr (Meta::is_residential_type(Type))
        {
            return "Residential";
        }
        else if constexpr (Meta::is_road_type(Type))
        {
            return "Infrastructure";
        }
        else
        {
            return "Unknown";
        }
    }

    /**
     * @brief Get resource production rate based on building type (compile-time)
     */
    template<BuildingType Type>
    constexpr int GetProductionRate()
    {
        if constexpr (Type == BuildingType::WOOD_WORKSHOP)
        {
            return 10; // Wood per second
        }
        else if constexpr (Type == BuildingType::ROCK_WORKSHOP)
        {
            return 8; // Rock per second
        }
        else if constexpr (Type == BuildingType::STEEL_WORKSHOP)
        {
            return 5; // Steel per second
        }
        else
        {
            return 0; // No production
        }
    }

    /**
     * @brief Get building capacity based on type (compile-time)
     */
    template<BuildingType Type>
    constexpr int GetBuildingCapacity()
    {
        if constexpr (Meta::is_residential_type(Type))
        {
            return 4; // 4 people per home
        }
        else if constexpr (Meta::is_workshop_type(Type))
        {
            return 2; // 2 workers per workshop
        }
        else
        {
            return 0;
        }
    }

    /**
     * @brief Example: Debug-only logging that gets completely removed in release builds
     */
    template<typename... Args>
    inline void DebugLog(Args&&... args)
    {
        if constexpr (Meta::is_debug_mode())
        {
            // This entire block is removed in release builds!
            (std::cout << ... << args) << std::endl;
        }
        // In release mode, this function becomes completely empty
        // The compiler will likely inline it away entirely
    }

    /**
     * @brief Example: Performance-critical code with debug checks
     */
    template<typename T>
    inline void ProcessData(T* data, size_t size)
    {
        // Bounds checking only in debug mode
        if constexpr (Meta::is_debug_mode())
        {
            if (data == nullptr)
            {
                std::cout << "ERROR: Null pointer passed to ProcessData!" << std::endl;
                return;
            }
            if (size == 0)
            {
                std::cout << "WARNING: Empty data array!" << std::endl;
                return;
            }
        }

        // This code runs in both debug and release
        for (size_t i = 0; i < size; ++i)
        {
            // Process data...
        }
    }

    /**
     * @brief Example usage demonstrating compile-time evaluation
     */
    inline void CompileTimeExamples()
    {
        // All of these are evaluated at COMPILE TIME!
        constexpr auto wood_category = GetBuildingCategory<BuildingType::WOOD_WORKSHOP>();
        constexpr auto home_category = GetBuildingCategory<BuildingType::HOME>();
        constexpr auto road_category = GetBuildingCategory<BuildingType::ROAD>();

        constexpr int wood_production = GetProductionRate<BuildingType::WOOD_WORKSHOP>();
        constexpr int rock_production = GetProductionRate<BuildingType::ROCK_WORKSHOP>();
        constexpr int steel_production = GetProductionRate<BuildingType::STEEL_WORKSHOP>();

        constexpr int home_capacity = GetBuildingCapacity<BuildingType::HOME>();
        constexpr int workshop_capacity = GetBuildingCapacity<BuildingType::WOOD_WORKSHOP>();

        // Debug logging - completely removed in release builds
        DebugLog("Wood Workshop Category: ", wood_category);
        DebugLog("Wood Production Rate: ", wood_production);
        DebugLog("Home Capacity: ", home_capacity);

        // Static assertions - compile-time checks!
        static_assert(wood_production == 10, "Wood production should be 10");
        static_assert(home_capacity == 4, "Home should hold 4 people");
        static_assert(Meta::is_workshop_type(BuildingType::WOOD_WORKSHOP), "Should be workshop");
        static_assert(!Meta::is_residential_type(BuildingType::WOOD_WORKSHOP), "Should not be residential");
    }

    /**
     * @brief Compile-time switch on building type
     * Usage: BuildingTypeSwitch<BuildingType::WOOD_WORKSHOP>::Execute();
     */
    template<BuildingType Type>
    struct BuildingTypeSwitch
    {
        static void Execute()
        {
            if constexpr (Type == BuildingType::WOOD_WORKSHOP)
            {
                // Code specific to wood workshop
                DebugLog("Processing Wood Workshop");
            }
            else if constexpr (Type == BuildingType::ROCK_WORKSHOP)
            {
                // Code specific to rock workshop
                DebugLog("Processing Rock Workshop");
            }
            else if constexpr (Type == BuildingType::STEEL_WORKSHOP)
            {
                // Code specific to steel workshop
                DebugLog("Processing Steel Workshop");
            }
            else if constexpr (Type == BuildingType::HOME)
            {
                // Code specific to home
                DebugLog("Processing Home");
            }
            else if constexpr (Type == BuildingType::ROAD)
            {
                // Code specific to road
                DebugLog("Processing Road");
            }
            else
            {
                // Default case
                DebugLog("Processing Unknown Building");
            }
        }
    };

} // namespace Dokev
