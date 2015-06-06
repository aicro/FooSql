/// @file Mutex.h
/// @brief The file defines a mutex for thread synchronization.

/// @author Aicro Ai
/// @date 2015/2/25

#ifndef COMMON_THREAD_MUTEX_H_
#define COMMON_THREAD_MUTEX_H_

#include <pthread.h>

namespace COMMON
{
    namespace THREAD
    {
        /// @brief A class represents a mutex.
        class Mutex
        {
            public:
                /// @brief Default constructor
                Mutex();
                
                /// @brief Lock the mutex
                void Lock();
                
                /// @brief Unlock the mutex
                void Unlock();
                
                ~Mutex();
                
                /// @brief get the raw mutex struct
                pthread_mutex_t* GetMutex();
                
            private:
                mutable pthread_mutex_t mutex;
        };
    }
}

#endif