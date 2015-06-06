/// @file Condition.h
/// @brief This file defines a condition variable

/// @author Aicro Ai
/// @date 2015/2/25

#ifndef COMMON_THREAD_CONDITION_H_
#define COMMON_THREAD_CONDITION_H_

#include <pthread.h>

namespace COMMON
{
    namespace THREAD
    {
        class Mutex;
    
        /// @brief This class represents a condition variable, which should be used with Mutex and MutexLockGuard.
        class Condition
        {
            public:
                /// @brief explicit default constructor
                explicit Condition();
                
                ~Condition();
                
                /// @brief Wait until signal to be sent
                /// @param mutex The mutex to prevent the condition variable be used in multiple threads simultaneously.
                void Wait(Mutex& mutex);
                
                /// @brief Notify one thread waiting in the same condition to continue.
                void Notify();
                
                /// @brief Notify all threads waiting in the same condition to continue.
                void NotifyAll();
                
            private:
                pthread_cond_t cond;
        };
    }
}

#endif