/**
 * \file DKSystemInfo.hpp
 * \brief System information utilities (RAM, CPU usage, etc.)
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include <cstdint>

namespace Dokev
{
    struct MemoryInfo
    {
        uint64_t totalPhysical = 0;      // Total physical RAM in bytes
        uint64_t availablePhysical = 0;  // Available physical RAM in bytes
        uint64_t usedPhysical = 0;       // Used physical RAM in bytes
        uint64_t processUsed = 0;        // RAM used by current process in bytes

        float GetUsagePercent() const
        {
            if (totalPhysical == 0) return 0.0f;
            return (static_cast<float>(usedPhysical) / static_cast<float>(totalPhysical)) * 100.0f;
        }

        float GetProcessUsageMB() const
        {
            return static_cast<float>(processUsed) / (1024.0f * 1024.0f);
        }

        float GetTotalPhysicalMB() const
        {
            return static_cast<float>(totalPhysical) / (1024.0f * 1024.0f);
        }

        float GetAvailablePhysicalMB() const
        {
            return static_cast<float>(availablePhysical) / (1024.0f * 1024.0f);
        }

        float GetUsedPhysicalMB() const
        {
            return static_cast<float>(usedPhysical) / (1024.0f * 1024.0f);
        }
    };

    class SystemInfo
    {
    public:
        // Get current memory information
        static MemoryInfo GetMemoryInfo();

        // Get memory info as formatted string
        static void GetMemoryInfoString(char* buffer, size_t bufferSize);
    };
}
