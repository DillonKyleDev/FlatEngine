#pragma once
// Resource used for thread pooling: https://www.geeksforgeeks.org/cpp/thread-pool-in-cpp/
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


namespace FlatEngine
{
	class ThreadPool
	{
    public:
        ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
        ~ThreadPool();

        void EnqueueTask(std::function<void()> task);
        void JoinThreads();

    private:
        void InitThreads();

        std::vector<std::thread> m_threads;
        size_t m_threadCount;
        std::queue<std::function<void()>> m_tasks;
        std::mutex m_queueMutex;                      // Mutex to synchronize access to shared data
        std::condition_variable m_conditionVariable;  // Condition variable to signal changes in the state of the tasks queue
        volatile bool m_b_stop = false;
	};
}

