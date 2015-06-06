#include "thread/MutexLockGuard.h"
#include "thread/Mutex.h"

namespace COMMON
{
    namespace THREAD
    {
        MutexLockGuard::MutexLockGuard(Mutex& mutex)
            :mutex_(mutex)
        {
            mutex_.Lock();
        }

        MutexLockGuard::~MutexLockGuard()
        {
            mutex_.Unlock();
        }
    }
}

