#include "WorkManager.hpp"
#include "Game/Entities/Human.hpp"

void WorkManager::RemoveJob(int jobId)
{
    for (int i = 0; i < (int)jobList.size(); ++i)
    {
        if (jobList[i].jobId == jobId)
        {
            Human* worker = (jobList[i].isAssigned && jobList[i].assignedWorker)
                                ? jobList[i].assignedWorker
                                : nullptr;

            jobList[i] = jobList.back();
            jobList.pop_back();
            if (worker)
                worker->CancelWork();

            return;
        }
    }
}

void WorkManager::RemoveAllJobsForBuilding(Building* building)
{
    for (int i = (int)jobList.size() - 1; i >= 0; --i)
    {
        if (jobList[i].targetBuilding == building)
        {
            Human* worker = (jobList[i].isAssigned && jobList[i].assignedWorker)
                                ? jobList[i].assignedWorker
                                : nullptr;

            jobList[i] = jobList.back();
            jobList.pop_back();

            if (worker)
                worker->CancelWork();
        }
    }
}
