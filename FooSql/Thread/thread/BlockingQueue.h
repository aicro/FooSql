/// @file BlockingQueue.h
/// @brief The file defines a bounded blocking queue using Mutex and Condition.

/// @author Aicro Ai
/// @date 2014/9/19

#ifndef THREAD_BLOCKINGQUEUE_H_
#define THREAD_BLOCKINGQUEUE_H_

#include <deque>
#include <pthread.h>
#include <vector>

#include "thread/Mutex.h"
#include "thread/MutexLockGuard.h"
#include "thread/Condition.h"

using namespace std;

namespace COMMON
{
    namespace THREAD
    {
        /// @brief A bounded blocking queue for threads synchronization.
        template<typename T>
        class BlockingQueue
        {
            public:
                /// @brief explicit constructor
                /// @param max_size the max size for the blocking queue
                explicit BlockingQueue(int max_size = 1024)
                    :mutex_(),full_(),empty_(),max_size_(max_size){}
       
                /// @brief Append an object to the end of the blocking queue. If the blocking queue is full, 
				/// the method will be blocked until a Pop() be called.
                /// @param a The object to be put into the blocking queue.
                void Push(T& a)
                {
                    MutexLockGuard guard(mutex_);
                    
                    while(queue_.size() == max_size_)
                    {
                        empty_.Wait(mutex_);
                    }
                    
                    queue_.push_back(a);
                    full_.Notify();
                }
                
                /// @brief Append multiple objects to the end of the blocking queue. 
				/// If the blocking queue is full, the method will be blocked until a Pop() be called.
                /// @param a The objects to be put into the blocking queue.
				void Push(vector<T>& a)
                {
                    MutexLockGuard guard(mutex_);
                    
                    while(queue_.size() + a.size() >= max_size_)
                    {
                        empty_.Wait(mutex_);
                    }
                    
                    for (int i = 0; i < a.size(); i++)
                    {
                        queue_.push_back(a[i]);
                    }
                    
                    full_.NotifyAll();
                }
                
				/// @brief Insert an object to the front of the blocking queue. 
				/// If the blocking queue is full, the method will be blocked until a Pop() be called.
                /// @param a The objects to be put into the blocking queue.
                void PushFront(T& a)
                {
                    MutexLockGuard guard(mutex_);
                    
                    while(queue_.size() == max_size_)
                    {
                        empty_.Wait(mutex_);
                    }
                    
                    queue_.push_front(a);
                    full_.Notify();
                }
                
                /// @brief Insert multiple objects to the front of the blocking queue. 
				/// If the blocking queue is full, the method will be blocked until a Pop() be called.
                /// @param a The objects to be put into the blocking queue.
				void PushFront(vector<T>& a)
                {
                    MutexLockGuard guard(mutex_);
                    
                    while(queue_.size() + a.size() >= max_size_)
                    {
                        empty_.Wait(mutex_);
                    }
                    
                    for (int i = a.size() - 1; i >= 0; i--)
                    {
                        queue_.push_front(a[i]);
                    }
                    
                    full_.NotifyAll();
                }

                /// @brief Get an object from the front of the blocking queue
                /// @param d The method will be blocked if no items have been put in the queue.
                void Pop(T& d)
                {
                    MutexLockGuard guard(mutex_);
                    
                    while(queue_.empty())
                    {
                        full_.Wait(mutex_);
                    }
                    
                    d = queue_.front();
                    queue_.pop_front();
                    
                    empty_.Notify();
                }

                /// @brief Testify whether a blocking queue is empty or not.
                /// @return true means the blocking queue is empty
                bool IsEmpty()
                {
                    MutexLockGuard guard(mutex_);
                    return queue_.empty();
                }

                /// @brief Testify whether a blocking queue is full or not.
                /// @return true means the blocking queue is full
                bool IsFull()
                {
                    MutexLockGuard guard(mutex_);
                    return queue_.size() == max_size_;
                }
                
                /// @brief Clear the blocking queue.
                void Clear()
                {
                    MutexLockGuard guard(mutex_);
                    queue_.clear();
                }
                
            private:
                Mutex mutex_;
                Condition full_;
                Condition empty_;
                deque<T> queue_;
                
                int max_size_;
        };
    
    }
}
#endif