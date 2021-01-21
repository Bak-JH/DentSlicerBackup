#pragma once
#include <mutex>
#include <condition_variable>

namespace Hix
{
    namespace Common
    {
        namespace Concurrency
        {
            class Semaphore
            {
            private:
                std::mutex _mutex;
                std::condition_variable _cv;
                size_t _count = 0; // Initialized as locked.

            public:
                Semaphore(size_t iniCount);
                void notify();
                void wait();
                bool try_wait();
#ifdef _DEBUG
                size_t count()const;
#endif
            };
        }
    }
}
