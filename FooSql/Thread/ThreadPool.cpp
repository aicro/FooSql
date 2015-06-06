#include <stdio.h>

#include "thread/ThreadPool.h"
#include "thread/Thread.h"
#include "thread/BlockingQueue.h"

#include "exception/ThrowableException.h"

//------------------------------------------------------------------------------
// ThreadPool
//------------------------------------------------------------------------------
namespace COMMON
{
    namespace THREAD
    {
        ThreadPool::ThreadPool( int threadNum, int maxQueueLength )
            : input_queue_(maxQueueLength), output_queue_(maxQueueLength)
        {   
            stoped_threads_num_ = 0;
            
            InitThreads(threadNum);

            running_ = false;
        }

        ThreadPool::~ThreadPool()
        {
        }

        void ThreadPool::InitThreads(int threadNum)
        {
            threads_.clear();
            threads_.resize(threadNum);
            for (int i = 0; i < threadNum; i++)
            {
                threads_[i].reset(new Thread());
            }
        }
        
        void ThreadPool::SetThreadsNum( int threadNum )
        {
            if (!running_)
            {
                InitThreads(threadNum);
            }
        }

        bool ThreadPool::SetCallbackForThreads( void* (*aFunc)(void*) )
        {
            if (!tFunc_)
            {
                tFunc_ = tr1::bind(aFunc, tr1::placeholders::_1);
                return true;
            }
            
            return false;
        }

        void * ThreadPool::wrapper_function(void * aArg)
        {
            ThreadPool* tp = (ThreadPool*) aArg;

			// all the threads in the thread pool are using 
			// the same input and output blocking queue.
            BlockingQueue<ThreadPoolTask>& inQueue = tp->input_queue_;
            BlockingQueue<ThreadPoolTask>& outQueue = tp->output_queue_;

            tr1::function<void* (void*)>& func = tp->tFunc_;

            ThreadPoolTask tpTask;
            
            do
            {
                inQueue.Pop(tpTask);
                
				// testify whether a stop signal has been met
                if (tpTask.to_stop_ == true)
                {
                    if (tpTask.clear_all_undo_tasks_)
                    {
                        inQueue.Clear();
                    }
                    
                    if (tpTask.clear_all_untaken_rslts_)
                    {
                        outQueue.Clear();
                    }
                    
                    break;
                }
                else
                {
					// do the real work
                    try
                    {
                        void* rslt = func(tpTask.task_);
                        
                        ThreadPoolTask t(rslt);
                        outQueue.Push(t);
                    }
                    catch (const COMMON::EXCEPTION::ThrowableException& ex)
                    {
                        fprintf(stderr, "An exception occurs in the thread %d\n", THIS_THREAD::GetId());
                        fprintf(stderr, "Details are %s\n", ex.What().c_str());
                        
                        std::terminate();
                    }
                    catch (const std::exception& ex)
                    {
                        fprintf(stderr, "An exception occurs in the thread %d\n", THIS_THREAD::GetId());
                        fprintf(stderr, "Details are %s\n", ex.what());
                        
                        std::terminate();
                    }
                    catch (...)
                    {
						fprintf(stderr, "An unknown exception occurs in the thread %d\n", THIS_THREAD::GetId());

                        throw; // re-throw
                    }
                }
                
            } while(1);

            // the end of the thread
            {
                MutexLockGuard lock(tp->stop_mutex_);
                tp->stoped_threads_num_++;
                
                tp->stop_condition_.Notify();
            }
            
            
            return 0;
        }

        vector<int> ThreadPool::Start()
        {
            vector<int> t_ids;
            if (!running_)
            {
                for (int i = 0; i < threads_.size(); i++)
                {
                    threads_[i]->SetThreadFuncInfo(wrapper_function, this);
                    threads_[i]->Start();
                    threads_[i]->Detach();
                    
                    int t_id = threads_[i]->GetId();
                    t_ids.push_back(t_id);
                }

                running_ = true;
            }
            
            return t_ids;
        }

        void ThreadPool::Stop(bool clearAllUndoTasks, bool clearUntakenRslt)
        {
            if (running_)
            {
                vector<ThreadPoolTask> tasks = 
                    PrepareStopTasks(clearAllUndoTasks, clearUntakenRslt);
                    
                PushTask(tasks);
                
                running_ = false;
            }
        }

        void ThreadPool::FastStop(bool clearAllUndoTasks, bool clearUntakenRslt)
        {
            if (running_)
            {
                vector<ThreadPoolTask> tasks = 
                    PrepareStopTasks(clearAllUndoTasks, clearUntakenRslt);
            
                PushTaskFront(tasks);

                running_ = false;
            }
        }

        vector<ThreadPoolTask> ThreadPool::PrepareStopTasks(bool clearAllUndoTasks, bool clearUntakenRslt)
        {
            vector<ThreadPoolTask> tasks;
            
            ThreadPoolTask task = PrepareStop(false, false);
            
            for (int i = 0; i < threads_.size(); i++)
            {
                // the last signal to inform stop
                if (i + 1 == threads_.size())
                {
                    task = PrepareStop(clearAllUndoTasks, clearUntakenRslt);
                }
                tasks.push_back(task);
            } 
            
            return tasks;
        }

        ThreadPoolTask ThreadPool::PrepareStop(bool clearAllUndoTasks, bool clearUntakenRslt)
        {
            ThreadPoolTask task;
            task.to_stop_ = true;
            task.clear_all_undo_tasks_ = clearAllUndoTasks;
            task.clear_all_untaken_rslts_ = clearUntakenRslt;
            
            return task;
        }

        bool ThreadPool::PushTask( ThreadPoolTask task )
        {
            if (running_ == false)
            {
                return false;
            }

            input_queue_.Push(task);
            return true;
        }

        bool ThreadPool::PushTask(vector<ThreadPoolTask> tasks)
        {
            if (running_ == false)
            {
                return false;
            }

            input_queue_.Push(tasks);
            return true;
        }

        bool ThreadPool::PushTaskFront( ThreadPoolTask task )
        {
            if (running_ == false)
            {
                return false;
            }

            input_queue_.PushFront(task);
            return true;
        }

        bool ThreadPool::PushTaskFront( vector<ThreadPoolTask> tasks )
        {
            if (running_ == false)
            {
                return false;
            }

            input_queue_.PushFront(tasks);
            return true;
        }

        void* ThreadPool::PopResult()
        {
            ThreadPoolTask t;
            output_queue_.Pop(t);
            
            return t.task_;
        }

        void ThreadPool::CancelAllUnrunTasks()
        {
            input_queue_.Clear();
        }

        void ThreadPool::ClearReadyRslts()
        {
            output_queue_.Clear();
        }        
        
        void ThreadPool::WaitAllThreadsEnd()
        {
            MutexLockGuard guard(stop_mutex_);
            while (stoped_threads_num_ != threads_.size())
            {
                stop_condition_.Wait(stop_mutex_);
            }
        }
    }
}
