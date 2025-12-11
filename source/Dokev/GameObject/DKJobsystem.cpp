#include "DKJobsystem.hpp"


void JobSystem::DoJob(JobSystem::Job job)
{
    {
        std::unique_lock<std::mutex> lock(m);
        job_queue.push(job);
        ++numberOfJobs;
    }
    cv.notify_one();
}

void JobSystem::WorkerThread()
{
    while(!isDone)
    {
        Job current_job;
        bool has_job = false;

        {
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock, [this]() { return !job_queue.empty() || isDone; });

            if(!job_queue.empty())
            {
                current_job = job_queue.front();
                job_queue.pop();
                has_job = true;
            }
        }

        if(has_job)
        {
            current_job();
            {
                std::unique_lock<std::mutex> lock(m);
                --numberOfJobs;
            }
        }
    }
}

void JobSystem::WaitUntilDone()
{
    while(numberOfJobs > 0)
    {
        Job current_job;
        bool has_job = false;

        {
            std::unique_lock<std::mutex> lock(m);
            if(!job_queue.empty())
            {
                current_job = job_queue.front();
                job_queue.pop();
                has_job = true;
            }
        }

        if(has_job)
        {
            current_job();
            {
                std::unique_lock<std::mutex> lock(m);
                --numberOfJobs;
            }
        }
        else
        {
            std::this_thread::yield();
        }
    }
}

void JobSystem::DoJobs(int how_many, JobSystem::ComputeAtIndex compute)
{
    int elements_per_group = how_many / num_threads;
    int remainder = how_many % num_threads;
    int start_index = 0;
    for(int j = 0; j < num_threads; ++j)
    {
        int task_total = elements_per_group;
        if(j < remainder)
        {
            task_total += 1;
        }
        int end_index = start_index + task_total;
        DoJob(
        [start_index, end_index, &compute] ()
        {
            for(int k = start_index; k < end_index; ++k)
            {
                compute(k);
            }
        });
        start_index = end_index;
    }
}

JobSystem::JobSystem()
{
    unsigned num_hw_threads = std::thread::hardware_concurrency();
    num_workers = num_hw_threads - 1;
    num_threads = static_cast<int>(num_hw_threads);
    t.resize(num_workers);

    for(size_t i = 0; i < num_workers; i++)
    {
        t[i] = std::thread(&JobSystem::WorkerThread, this);
    }
}

JobSystem::~JobSystem()
{
    isDone = true;
    cv.notify_all();

    for(size_t i = 0; i < num_workers; i++)
    {
        if(t[i].joinable())
        {
            t[i].join();
        }
    }
}
