/// @file Thread.h
/// @brief The file defines a class representing a thread

/// @author Aicro Ai
/// @date 2015/2/25

#ifndef COMMON_TREAD_THREAD_H_
#define COMMON_TREAD_THREAD_H_

#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include "thread/Mutex.h"
#include "thread/Condition.h"

namespace COMMON 
{
    namespace THREAD 
    {
        /// @brief A class representing a thread
        class Thread 
        {
        public:
            class ThreadInfo;

            /// @brief Default constructor. This method will create a thread object, 
			/// but not running right now. 
            Thread();

            /// @brief Constructor specifying thread callback function and parameter.
            /// @param[in] aFunction thread callback function
            /// @param[in] aArg parameter for thread
            Thread(void* (*aFunction)(void *), void * aArg);

            /// @brief Deconstructor
            /// @note If the thread is joinable, the method will make it detached
            ~Thread();

            /// @brief Set callback function and parameter before start the thread
            void SetThreadFuncInfo(void* (*aFunction)(void *), void * aArg);

            /// @brief To test whether the thread is running
            bool IsRunning();

            /// @brief Start the thread.
            /// @return The return code is the same as that of pthread_create(), in which 0 means success, others are error codes.
            int Start();
            
            /// @brief Wait until the end of the thread.
            void Join();

            /// @brief To test whether a thread is joinable
            bool Joinable() const;

            /// @brief Detach the thread, which will make it non-joinable. All the resources 
			/// occupied by the thread will return to the OS after it exit.
            void Detach();

            /// @brief Get the thread ID
            /// @return The thread id
            int GetId() const;

            /// @brief Get native handle
            /// @return native handle
            inline pthread_t GetNativeHandle()
            {
                return handle_;
            }

            /// @brief 检查当前环境，以得到当前环境下可能会被同时执行的线程数量。
            /// 此函数在需要知道某个任务的最合理线程数时非常有用。
            /// @return 当前环境下可能会被同时执行的线程数量
            /// @note 如果此值在操作系统环境下不可用，则默认返回0
			/// @brief To get the most reasonable threads number under the current hardware environment.
			/// @return the most reasonable threads number under the current hardware environment
            static unsigned long HardwareConcurrency();

        private:
            Thread(const Thread&);
            Thread& operator=(const Thread&);
           
        private:
            pthread_t handle_;          ///< thread handle
            mutable Mutex data_mutex_;  ///< data mutex
            bool detatched_;            ///< True means detatched 
            bool running_;              ///< True means the thread is running
            int tId_;                   ///< the thread ID
            
            mutable Mutex tId_got_mutex_;  ///< the mutex for getting the thread id
            mutable Condition tId_got_condition_;  ///< the condition variable for getting the thread id
            
            static void * wrapper_function(void * aArg);

        private:
            /// @brief information for newly start thread
            struct _thread_start_info 
            {
                Condition* tId_got_condition_;   ///< the condition variable for getting the thread id
                Mutex* tId_got_mutex_;          ///< the mutex for getting the thread id
                int* tId_;                   ///< Thread ID
                void* (*function_)(void *); ///< callback function
                void* arg_;                 ///< parameter
                Thread * thread_;           ///< the parent @c Thread pointer
                
                _thread_start_info()
                    :function_(0), arg_(0), thread_(0), tId_(0), tId_got_mutex_(0), tId_got_condition_(0) {}
            };

            _thread_start_info ti_;
        };

        //------------------------------------------------------------------------------
        // Duration
        //------------------------------------------------------------------------------
        template <long long N, long long D = 1> 
        class ratio 
        {
        public:
            static double _as_double() { return double(N) / double(D); }
        };

        namespace CHRONO 
        {
            template <class _Period> 
            class duration 
            {
            private:
                long long rep_;

            public:
                explicit duration(const long long r) 
                    : rep_(r) {}

                long long count() const
                {
                    return rep_;
                }
            };

            // Standard duration types.
            typedef duration<ratio<1,    1000000000> > NanoSeconds;  
            typedef duration<ratio<1,    1000000> >    MicroSeconds; 
            typedef duration<ratio<1,    1000> >       MilliSeconds; 
            typedef duration<ratio<1,    1> >          Seconds;      
            typedef duration<ratio<60,   1> >          Minutes;      
            typedef duration<ratio<3600, 1> >          Hours;        
        }

        //------------------------------------------------------------------------------
        // THIS_THREAD
        //------------------------------------------------------------------------------
		/// @brief The namespace provides methods for the calling thread
        namespace THIS_THREAD 
        {
            /// thread id
            extern __thread int t_cachedTid;
        
            /// @brief Get the current thread's thread id
			/// @return thread id of the current thread.
            int GetId();

            /// @brief Test whether the current thread is a main thread.
			/// @return true means it is the main thread
            bool IsMainThread();
            
			/// @brief give up the remaining time, so that OS may schedule other threads to run.
            inline void Yield()
            {
                sched_yield();
            }

            /// @brief Let the current thread to sleep for a specified time. 
			/// @code
			/// // sleep for 100 milliseconds
            /// COMMON::THREAD::THIS_THREAD::SleepFor(COMMON::THREAD::CHRONO::MilliSeconds(100));
			/// @endcode
			/// @param aTime time for sleep
			template <class _Period> 
            void SleepFor(const CHRONO::duration<_Period>& aTime)
            {
                usleep(int(double(aTime.count()) * (1000000.0 * _Period::_as_double()) + 0.5));
            }
        }

    }

}
#endif
