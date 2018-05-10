// Copyright © 2016 zendo (734181759@qq.com). All rights reserved.
#pragma once
#include <assert.h>
#include <type_traits>
#include <limits>
#include <atomic>

#if defined(_WIN32)
#include <windows.h>// The windows.h is pollution
#pragma comment(lib,"Kernel32.lib")
extern "C" {
    struct _SECURITY_ATTRIBUTES;
    __declspec(dllimport) void* __stdcall CreateSemaphoreW(_SECURITY_ATTRIBUTES* lpSemaphoreAttributes, long lInitialCount, long lMaximumCount, const wchar_t* lpName);
    __declspec(dllimport) int __stdcall CloseHandle(void* hObject);
    __declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void* hHandle, unsigned long dwMilliseconds);
    __declspec(dllimport) int __stdcall ReleaseSemaphore(void* hSemaphore, long lReleaseCount, long* lpPreviousCount);
}
#elif defined(__MACH__)
#include <mach/mach.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <errno.h>
#include <time.h>
#elif defined(__unix__)
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#endif

namespace zendo
{
    
    namespace details
    {
        
#if defined(_WIN32)
        class semaphore_impl
        {
        private:
            void* impl_;
        public:
            semaphore_impl(const semaphore_impl&) = delete;
            semaphore_impl(int initial_count = 0)
            {
                assert(initial_count >= 0);
                impl_ = CreateSemaphoreW(nullptr, initial_count, (std::numeric_limits<long>::max)(), nullptr);
            }
            
            ~semaphore_impl()
            {
                CloseHandle(impl_);
            }
            
            void wait()
            {
                WaitForSingleObject(impl_, (std::numeric_limits<unsigned long>::max)());
            }
            
            bool try_wait()
            {
                return WaitForSingleObject(impl_, 0) != WAIT_TIMEOUT;
            }
            
            bool timed_wait(uint64_t usecs)
            {
                return WaitForSingleObject(impl_, (unsigned long)(usecs / 1000)) != WAIT_TIMEOUT;
            }
            
            void signal(int count = 1)
            {
                ReleaseSemaphore(impl_, count, nullptr);
            }
            
        };
        
#elif defined(__MACH__)// Semaphore (Apple iOS and OSX)
        class semaphore_impl
        {
        private:
            semaphore_t impl_;
        public:
            semaphore_impl(const semaphore_impl&) = delete;
            semaphore_impl(int initial_count = 0)
            {
                assert(initial_count >= 0);
                semaphore_create(mach_task_self(), &impl_, SYNC_POLICY_FIFO, initial_count);
            }

            ~semaphore_impl()
            {
                semaphore_destroy(mach_task_self(), impl_);
            }

            void wait()
            {
                semaphore_wait(impl_);
            }

            bool try_wait()
            {
                return timed_wait(0);
            }

            bool timed_wait(int64_t timeout_usecs)
            {
                mach_timespec_t ts;
                ts.tv_sec = (unsigned int)(timeout_usecs / 1000000);
                ts.tv_nsec = (timeout_usecs % 1000000) * 1000;
                kern_return_t rc = semaphore_timedwait(impl_, ts);
                return rc != KERN_OPERATION_TIMED_OUT;
            }

            void signal()
            {
                semaphore_signal(impl_);
            }

            void signal(int count)
            {
                while (count-- > 0)
                {
                    semaphore_signal(impl_);
                }
            }
        };
        
#elif defined(__unix__) // semaphore_impl (POSIX, Linux)
        class semaphore_impl
        {
        private:
            sem_t impl_;
        public:
            semaphore_impl(const semaphore_impl&) = delete;
            semaphore_impl(int initial_count = 0)
            {
                assert(initial_count >= 0);
                sem_init(&impl_, 0, initial_count);
            }
            
            ~semaphore_impl()
            {
                sem_destroy(&impl_);
            }
            
            void wait()
            {
                int rc;
                do
                {
                    rc = sem_wait(&impl_);
                } while (rc == -1 && errno == EINTR);
            }
            
            bool try_wait()
            {
                int rc;
                do
                {
                    rc = sem_trywait(&impl_);
                } while (rc == -1 && errno == EINTR);
                return !(rc == -1 && errno == EAGAIN);
            }
            
            bool timed_wait(uint64_t usecs)
            {
                struct timespec ts;
                const int usecs_in_1_sec = 1000000;
                const int nsecs_in_1_sec = 1000000000;
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += usecs / usecs_in_1_sec;
                ts.tv_nsec += (usecs % usecs_in_1_sec) * 1000;
                // sem_timedwait bombs if you have more than 1e9 in tv_nsec so we have to clean things up before passing it in
                if (ts.tv_nsec > nsecs_in_1_sec)
                {
                    ts.tv_nsec -= nsecs_in_1_sec;
                    ++ts.tv_sec;
                }
                int rc;
                do
                {
                    rc = sem_timedwait(&impl_, &ts);
                } while (rc == -1 && errno == EINTR);
                return !(rc == -1 && errno == ETIMEDOUT);
            }
            
            void signal()
            {
                sem_post(&impl_);
            }
            
            void signal(int count)
            {
                while (count-- > 0)
                {
                    sem_post(&impl_);
                }
            }
        };
#endif
        
    }//namespace details
    
    class semaphore
    {
    public:
        using ssize_t = typename std::make_signed<size_t>::type;
    private:
        details::semaphore_impl impl_;
        std::atomic<ssize_t> count_;
        
        bool wait_with_partial_spinning(int64_t timeout_usecs = -1)
        {
            int spin = 10000;
            while (--spin >= 0)
            {
                if (count_.load() > 0)
                {
                    count_.fetch_sub(1, std::memory_order_acquire);
                    return true;
                }
                // Prevent the compiler from collapsing the loop.
                std::atomic_signal_fence(std::memory_order_acquire);
            }
            ssize_t old_count = count_.fetch_sub(1, std::memory_order_acquire);
            if (old_count > 0)
            {
                return true;
            }
            if (impl_.timed_wait(timeout_usecs))
            {
                return true;
            }
            while (true)
            {
                old_count = count_.fetch_add(1, std::memory_order_release);
                if (old_count < 0)
                {
                     return false;
                }
                old_count = count_.fetch_sub(1, std::memory_order_acquire);
                if (old_count > 0 && impl_.try_wait())
                {
                    return true;
                }
            }
        }
    public:
        semaphore(const semaphore&) = delete;
        semaphore(ssize_t initial_count = 0)
        : count_(initial_count)
        {
            assert(initial_count >= 0);
        }
        
        bool try_wait()
        {
            if (count_.load() > 0)
            {
                count_.fetch_sub(1, std::memory_order_acquire);
                return true;
            }
            return false;
        }
        
        void wait()
        {
            if (!try_wait())
            {
                wait_with_partial_spinning();
            }
        }
        
        bool wait(int64_t timeout_usecs)
        {
            return try_wait() || wait_with_partial_spinning(timeout_usecs);
        }
        
        void signal(ssize_t count = 1)
        {
            assert(count >= 0);
            const ssize_t old_count = count_.fetch_add(count, std::memory_order_release);
            assert(old_count >= -1);
            if (old_count < 0)
            {
                impl_.signal(1);
            }
        }
        
        ssize_t available_approx() const
        {
            ssize_t count = count_.load();
            return count > 0 ? count : 0;
        }
        
    };
    
}

