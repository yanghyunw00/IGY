/**
 * \file WorkManager.hpp
 * \brief
 * \author
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Engine/Vec2.hpp"
#include <vector>
#include <algorithm>

class Human;
class Building; 

enum class WorkType;

struct Job
{
    int         jobId;
    WorkType    type;
    Math::fvec2 position;
    Building* targetBuilding;

    int    priority; // higher = filled first

    bool   isAssigned;
    Human* assignedWorker;

    Job(int id, WorkType t, Math::fvec2 pos, Building* b, int prio = 0)
        : jobId(id), type(t), position(pos), targetBuilding(b), priority(prio), isAssigned(false), assignedWorker(nullptr) {}
};

class WorkManager
{
private:
    std::vector<Job> jobList;
    int nextJobId = 1;

    WorkManager() { jobList.reserve(200); }

public:
    static WorkManager& GetInstance()
    {
        static WorkManager instance;
        return instance;
    }

    // Called by Building
    void AddJob(WorkType type, Math::fvec2 position, Building* building, int priority = 0)
    {
        jobList.push_back(Job(nextJobId++, type, position, building, priority));
    }

    // Called by Human
    bool RequestJob(Human* requester, Math::fvec2 humanPos, int& outJobId, Math::fvec2& outJobPos, WorkType& outType, float maxRadius = 20.0f)
    {
        const float maxDistSq = maxRadius * maxRadius;
        int   bestIndex    = -1;
        int   bestPriority = -1;
        float bestDistSq   = maxDistSq;

        for (int i = 0; i < static_cast<int>(jobList.size()); ++i)
        {
            if (jobList[i].isAssigned)
                continue;

            float dx     = jobList[i].position.x - humanPos.x;
            float dy     = jobList[i].position.y - humanPos.y;
            float distSq = (dx * dx) + (dy * dy);

            if (distSq >= maxDistSq)
                continue; // out of search range

            // priority first, then nearest
            if (jobList[i].priority > bestPriority ||
                (jobList[i].priority == bestPriority && distSq < bestDistSq))
            {
                bestPriority = jobList[i].priority;
                bestDistSq   = distSq;
                bestIndex    = i;
            }
        }

        if (bestIndex != -1)
        {
            jobList[bestIndex].isAssigned     = true;
            jobList[bestIndex].assignedWorker = requester;

            outJobId  = jobList[bestIndex].jobId;
            outJobPos = jobList[bestIndex].position;
            outType   = jobList[bestIndex].type;
            return true;
        }
        return false;
    }

    void RemoveJob(int jobId);

    // Remove job from list without notifying the assigned worker (use when worker has already arrived)
    void EraseJob(int jobId)
    {
        for (int i = 0; i < static_cast<int>(jobList.size()); ++i)
        {
            if (jobList[i].jobId == jobId)
            {
                jobList[i] = jobList.back();
                jobList.pop_back();
                return;
            }
        }
    }

    // When destroyed building
    void RemoveAllJobsForBuilding(Building* building);

    // worker leaves 
    void UnassignJob(int jobId)
    {
        for (auto& job : jobList)
        {
            if (job.jobId == jobId)
            {
                job.isAssigned     = false;
                job.assignedWorker = nullptr;
                return;
            }
        }
    }

    Job* GetJob(int jobId)
    {
        for (auto& job : jobList)
            if (job.jobId == jobId)
                return &job;
        return nullptr;
    }

    const std::vector<Job>& GetJobs() const { return jobList; }
    int                     GetJobCount() const { return static_cast<int>(jobList.size()); }

    void ClearAllJobs() { jobList.clear(); }
};