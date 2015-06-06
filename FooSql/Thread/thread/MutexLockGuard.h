/// @file MutexLockGuard.h
/// @brief A helper class to simplify entering and leaving critical section.

/// @author Aicro Ai
/// @date 2015/2/25

#ifndef COMMON_THREAD_MUTEX_LOCK_GUARD_H_
#define COMMON_THREAD_MUTEX_LOCK_GUARD_H_

namespace COMMON
{
    namespace THREAD
    {
        class Mutex;
    
        /// @brief A class to simplify entering and leaving critical sections.
        class MutexLockGuard
        {
            public:
                /// @brief The explicit constructor
                /// @param mutex The mutex to synchronization
                ///  
                /// Usage:
                /// \code
                ///	Mutex mutex;
                /// {
                ///     MutexLockGuard(mutex);
                ///     // you have entered a critical section
                /// }
                /// // you have leaved that critical section
                /// \endcode
                explicit MutexLockGuard(Mutex& mutex);
                
                /// @brief Deconstructor. Leaving the critical section
                ~MutexLockGuard();
                
            private:
                Mutex& mutex_;
        };
    }
}

#endif