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

            if (worker)
                worker->CancelWork();

            for (int j = 0; j < (int)jobList.size(); ++j)
            {
                if (jobList[j].jobId == jobId)
                {
                    jobList[j] = jobList.back();
                    jobList.pop_back();
                    break;
                }
            }
            return;
        }
    }
}

void WorkManager::RemoveAllJobsForBuilding(Building* building)
{
    std::vector<Human*> workersToCancel;
    for (auto& job : jobList)
    {
        if (job.targetBuilding == building && job.isAssigned && job.assignedWorker)
            workersToCancel.push_back(job.assignedWorker);
    }

    // Erase jobs first so CancelWork can't call back into the dying building
    for (int i = (int)jobList.size() - 1; i >= 0; --i)
    {
        if (jobList[i].targetBuilding == building)
        {
            jobList[i] = jobList.back();
            jobList.pop_back();
        }
    }

    for (Human* worker : workersToCancel)
        worker->CancelWork();
}
