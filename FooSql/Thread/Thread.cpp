#include <stdlib.h>
#include <exception>
#include <unistd.h>

#define _GNU_SOURCE        // or _BSD_SOURCE or _SVID_SOURCE
#include <sys/syscall.h>   // For SYS_xxx definitions

#include <stdio.h>

#include "thread/Thread.h"
#include "thread/MutexLockGuard.h"

#include "exception/ThrowableException.h"

namespace COMMON
{
    namespace THREAD
    {
        namespace THIS_THREAD
        {
            __thread int t_cachedTid = -1;
        }
    
        // 根据陈硕muduo的建议
        // Apply the suggestions from Chen Shuo that a thread in Linux is a kind of light process,
		// Therefore, a pid can represent a thread better than pthread_t, which is unique in a
		// process but not the system.
        pid_t gettid()
        {
            return static_cast<pid_t>(::syscall(SYS_gettid));
        }
        
        // provide name for the main thread after a call to fork()
        void afterFork()
        {
            COMMON::THREAD::THIS_THREAD::GetId();
            // no need to call pthread_atfork(0, 0, &afterFork);
        }
        
        // provide name and thread id for the main thread
        class ThreadNameInitializer
        {
        public:
            ThreadNameInitializer()
            {
                COMMON::THREAD::THIS_THREAD::GetId();
                pthread_atfork(0, 0, &afterFork);
            }
        };
        
        // use a global variety to give name and thread id to the main thread
        ThreadNameInitializer init;
    }
}

//------------------------------------------------------------------------------
// Thread
//------------------------------------------------------------------------------

// a wrapper function represents the real thread function.
void * COMMON::THREAD::Thread::wrapper_function(void * aArg)
{
    _thread_start_info * ti_ = (_thread_start_info *) aArg;

    // get thread ID
    {
        MutexLockGuard g(*(ti_->tId_got_mutex_));
        *(ti_->tId_) = COMMON::THREAD::THIS_THREAD::GetId();
        ti_->tId_got_condition_->Notify();
    }
    
    try
    {
        ti_->function_(ti_->arg_);
    }
    catch (const COMMON::EXCEPTION::ThrowableException& ex)
    {
		fprintf(stderr, "An exception has been met in the thread %d\n", COMMON::THREAD::THIS_THREAD::GetId());
		fprintf(stderr, "Details are %s\n", ex.What().c_str());
		
		std::terminate();
    }
    catch (const std::exception& ex)
    {
		fprintf(stderr, "An exception has been met in the thread %d\n", COMMON::THREAD::THIS_THREAD::GetId());
		fprintf(stderr, "Details are %s\n", ex.what());
        
		std::terminate();
    }
    catch (...)
    {
        fprintf(stderr, "An unknown exception has been met in the thread %d\n", COMMON::THREAD::THIS_THREAD::GetId());

        throw; // re-throw
    }

    {
        COMMON::THREAD::MutexLockGuard guard(ti_->thread_->data_mutex_);
        ti_->thread_->running_ = false;
    }

    return 0;
}

COMMON::THREAD::Thread::Thread() 
    : handle_(0), detatched_(false), tId_(-1), running_(false)
{
}

COMMON::THREAD::Thread::Thread(void* (*aFunction)(void *), void * aArg)
    :tId_(-1), detatched_(false), running_(false)
{
    SetThreadFuncInfo(aFunction, aArg);
}

void COMMON::THREAD::Thread::SetThreadFuncInfo(void* (*aFunction)(void *), void * aArg)
{
    if (!running_)
    {
        ti_.function_ = aFunction;
        ti_.arg_ = aArg;
        ti_.thread_ = this;
        ti_.tId_ = &tId_;
        ti_.tId_got_mutex_ = &tId_got_mutex_;
        ti_.tId_got_condition_ = &tId_got_condition_;
    }
}

COMMON::THREAD::Thread::~Thread()
{
    // detach the thread so that it can be isolated from the object.
    bool rslt = false;
    {
        COMMON::THREAD::MutexLockGuard guard(data_mutex_);
        rslt = running_;
    }

    if(rslt && Joinable())
    {
        pthread_detach(handle_);
        detatched_ = false;
    }
}

bool COMMON::THREAD::Thread::IsRunning()
{
    bool rslt = false;
    {
        COMMON::THREAD::MutexLockGuard guard(data_mutex_);
        rslt = running_;
    }
    return rslt;
}

int COMMON::THREAD::Thread::Start()
{
    int error = 0;
    
    if (!running_)
    {
        error = pthread_create(&handle_, 0, wrapper_function, (void *) &ti_);
        if(error != 0)
        {
            handle_ = 0;
            running_ = false;
        }
        else
        {
            {
                COMMON::THREAD::MutexLockGuard guard(data_mutex_);
                detatched_ = false;
                running_ = true;    
            }            
            
            // Wait thread to be correctly given name and thread id
            {
                COMMON::THREAD::MutexLockGuard guard(tId_got_mutex_);
                while(tId_ == -1)
                {
                    tId_got_condition_.Wait(tId_got_mutex_);
                }
            }
        }
    }
    
    return error;
}

void COMMON::THREAD::Thread::Join()
{
    if(Joinable())
    {
        pthread_join(handle_, 0);
    }
    
    COMMON::THREAD::MutexLockGuard guard(data_mutex_);
    handle_ = 0;
    running_ = false;
    detatched_ = false;
}

bool COMMON::THREAD::Thread::Joinable() const
{
    bool result = false;
    {
        COMMON::THREAD::MutexLockGuard guard(data_mutex_);
        result = !detatched_;
    }

    return result;
}

void COMMON::THREAD::Thread::Detach()
{
    COMMON::THREAD::MutexLockGuard guard(data_mutex_);
    if(!detatched_)
    {
        pthread_detach(handle_);
        detatched_ = true;
    }
}

int COMMON::THREAD::Thread::GetId() const
{
    return tId_;
}

unsigned long COMMON::THREAD::Thread::HardwareConcurrency()
{
#if defined(_SC_NPROCESSORS_ONLN)
    return (int) sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
    return (int) sysconf(_SC_NPROC_ONLN);
#else
    return 0;
#endif
}

//------------------------------------------------------------------------------
// THIS_THREAD
//------------------------------------------------------------------------------
int COMMON::THREAD::THIS_THREAD::GetId()
{
    if (t_cachedTid == -1)
    {
        t_cachedTid = static_cast<int>(COMMON::THREAD::gettid());
    }
    
    return t_cachedTid;
}

bool COMMON::THREAD::THIS_THREAD::IsMainThread()
{
    return COMMON::THREAD::THIS_THREAD::GetId() == ::getpid();
}
