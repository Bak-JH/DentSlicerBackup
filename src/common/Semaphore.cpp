#include "Semaphore.h"

Hix::Common::Concurrency::Semaphore::Semaphore(size_t iniCount):_count(iniCount)
{
}

void Hix::Common::Concurrency::Semaphore::notify()
{
    std::lock_guard<decltype(_mutex)> lock(_mutex);
    ++_count;
    _cv.notify_one();
}

void Hix::Common::Concurrency::Semaphore::wait()
{
    std::unique_lock<decltype(_mutex)> lock(_mutex);
    while (!_count) // Handle spurious wake-ups.
        _cv.wait(lock);
    --_count;
}

bool Hix::Common::Concurrency::Semaphore::try_wait()
{
    std::lock_guard<decltype(_mutex)> lock(_mutex);
    if (_count) {
        --_count;
        return true;
    }
    return false;
}

#ifdef _DEBUG

size_t Hix::Common::Concurrency::Semaphore::count() const
{
    return _count;
}

#endif