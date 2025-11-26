#pragma once
#include "DKJobsystem.hpp"

class JobSystemManager {
private:
    JobSystem job_system;
    JobSystemManager() = default;
    
public:

    JobSystemManager(const JobSystemManager&) = delete;
    JobSystemManager& operator=(const JobSystemManager&) = delete;

    static JobSystemManager& GetInstance()
    {
        static JobSystemManager instance;
        return instance;
    }
    
    JobSystem& GetJobSystem() { return job_system; }
};
