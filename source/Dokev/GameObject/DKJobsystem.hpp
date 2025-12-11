#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <atomic>


class JobSystem
{
public:
    using Job = std::function<void(void)>;
    /**
     * @brief Adds a job to the job queue
     * @param job 
     * @return none
     */
    void DoJob(Job job);
    
    /**
     * @brief Waits until all jobs are completed
     */
    void WaitUntilDone();

    using ComputeAtIndex = std::function<void(int)>;

    /**
     * @brief Divides the work among available threads and executes the compute function for each index
     * @param how_many 
     * @param compute 
     * @return none
     */
    void DoJobs(int how_many, ComputeAtIndex compute);

public:
    /**
    * @brief Constructor
    */
    JobSystem();

    /**
    * @brief Destructor
    */
    ~JobSystem(); // RAII
public:
    // Rule of 5
    JobSystem(const JobSystem&)            = delete;
    JobSystem(JobSystem&&)                 = delete;
    JobSystem& operator=(const JobSystem&) = delete;
    JobSystem& operator=(JobSystem&&)      = delete;

private:
    // helper function(s)

    /**
     * @brief Function that represents the worker thread's main loop.
     * @param none
     * @return none
     */
    void WorkerThread();
    // Job GetJob() { return }

private:
    // fields
    std::vector<std::thread> t;
    std::mutex               m;
    std::condition_variable cv;
    std::queue<Job>         job_queue;

    std::atomic_bool isDone  = false;

    std::atomic_int numberOfJobs = 0;
    size_t num_workers  = 0;
    int num_threads  = 0;
};
