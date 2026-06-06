/**
 * \file DKSystemInfo.cpp
 * \brief System information utilities implementation
 * \author Dongyun Lee
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKSystemInfo.hpp"
#include <cstdio>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <psapi.h>
    #pragma comment(lib, "psapi.lib")
#elif defined(__linux__)
    #include <sys/sysinfo.h>
    #include <unistd.h>
    #include <fstream>
    #include <string>
#elif defined(__APPLE__)
    #include <mach/mach.h>
    #include <sys/sysctl.h>
#endif

namespace Dokev
{
    MemoryInfo SystemInfo::GetMemoryInfo()
    {
        MemoryInfo info;

#if defined(_WIN32) || defined(_WIN64)
        // Windows implementation
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);
        if (GlobalMemoryStatusEx(&memStatus))
        {
            info.totalPhysical = memStatus.ullTotalPhys;
            info.availablePhysical = memStatus.ullAvailPhys;
            info.usedPhysical = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
        }

        // Get process memory usage
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        {
            info.processUsed = pmc.WorkingSetSize;
        }

#elif defined(__linux__)
        // Linux implementation
        struct sysinfo si;
        if (sysinfo(&si) == 0)
        {
            info.totalPhysical = si.totalram * si.mem_unit;
            info.availablePhysical = si.freeram * si.mem_unit;
            info.usedPhysical = info.totalPhysical - info.availablePhysical;
        }

        // Get process memory usage from /proc/self/status
        std::ifstream statusFile("/proc/self/status");
        std::string line;
        while (std::getline(statusFile, line))
        {
            if (line.find("VmRSS:") == 0)
            {
                // VmRSS is in kB
                size_t pos = line.find_first_of("0123456789");
                if (pos != std::string::npos)
                {
                    uint64_t kb = std::stoull(line.substr(pos));
                    info.processUsed = kb * 1024;
                }
                break;
            }
        }

#elif defined(__APPLE__)
        // macOS implementation
        int mib[2];
        int64_t physical_memory;
        size_t length;

        // Total physical memory
        mib[0] = CTL_HW;
        mib[1] = HW_MEMSIZE;
        length = sizeof(int64_t);
        sysctl(mib, 2, &physical_memory, &length, NULL, 0);
        info.totalPhysical = physical_memory;

        // Available memory
        vm_size_t page_size;
        mach_port_t mach_port;
        mach_msg_type_number_t count;
        vm_statistics64_data_t vm_stats;

        mach_port = mach_host_self();
        count = sizeof(vm_stats) / sizeof(natural_t);
        if (host_page_size(mach_port, &page_size) == KERN_SUCCESS &&
            host_statistics64(mach_port, HOST_VM_INFO, reinterpret_cast<host_info64_t>(&vm_stats), &count) == KERN_SUCCESS)
        {
            uint64_t free_memory = static_cast<uint64_t>(vm_stats.free_count) * static_cast<uint64_t>(page_size);
            info.availablePhysical = free_memory;
            info.usedPhysical = info.totalPhysical - free_memory;
        }

        // Process memory usage
        struct task_basic_info t_info;
        mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

        if (task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&t_info), &t_info_count) == KERN_SUCCESS)
        {
            info.processUsed = t_info.resident_size;
        }

#else
        // Unsupported platform - return zeros
        info.totalPhysical = 0;
        info.availablePhysical = 0;
        info.usedPhysical = 0;
        info.processUsed = 0;
#endif

        return info;
    }

    void SystemInfo::GetMemoryInfoString(char* buffer, std::size_t bufferSize)
    {
        MemoryInfo info = GetMemoryInfo();

        std::snprintf(buffer, bufferSize,
            "RAM: %.1f MB / %.1f MB (%.1f%%) | Process: %.1f MB",
            static_cast<double>(info.GetUsedPhysicalMB()),
            static_cast<double>(info.GetTotalPhysicalMB()),
            static_cast<double>(info.GetUsagePercent()),
            static_cast<double>(info.GetProcessUsageMB())
        );
    }
}
