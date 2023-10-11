
#pragma once

#include <pthread.h>
#include <thread>
#include <string>
#include <functional>
#include <memory>
#include <semaphore.h>
#include <atomic>

#include "./noncopyable.h"

namespace frb{

    //信号量
    class Semaphore : Noncopyable{
        public:
            //val 初始值
            Semaphore(uint32_t val = 0);
            ~Semaphore();

            void wait();
            void notify();

        private:
            sem_t m_semaphore;
    };
    /**
     * @brief 局部锁的模板实现
     */
    template<class T>
    struct ScopedLockImpl {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex Mutex
         */
        ScopedLockImpl(T& mutex)
            :m_mutex(mutex) {
            m_mutex.lock();
            m_locked = true;
        }

        /**
         * @brief 析构函数,自动释放锁
         */
        ~ScopedLockImpl() {
            unlock();
        }

        /**
         * @brief 加锁
         */
        void lock() {
            if(!m_locked) {
                m_mutex.lock();
                m_locked = true;
            }
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            if(m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        /// mutex
        T& m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

    /**
     * @brief 局部读锁模板实现
     */
    template<class T>
    struct ReadScopedLockImpl {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex 读写锁
         */
        ReadScopedLockImpl(T& mutex)
            :m_mutex(mutex) {
            m_mutex.rdlock();
            m_locked = true;
        }

        /**
         * @brief 析构函数,自动释放锁
         */
        ~ReadScopedLockImpl() {
            unlock();
        }

        /**
         * @brief 上读锁
         */
        void lock() {
            if(!m_locked) {
                m_mutex.rdlock();
                m_locked = true;
            }
        }

        /**
         * @brief 释放锁
         */
        void unlock() {
            if(m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        /// mutex
        T& m_mutex;
        /// 是否已上锁
        bool m_locked;
    };

    /**
     * @brief 局部写锁模板实现
     */
    template<class T>
    struct WriteScopedLockImpl {
    public:
        /**
         * @brief 构造函数
         * @param[in] mutex 读写锁
         */
        WriteScopedLockImpl(T& mutex)
            :m_mutex(mutex) {
            m_mutex.wrlock();
            m_locked = true;
        }

        /**
         * @brief 析构函数
         */
        ~WriteScopedLockImpl() {
            unlock();
        }

        /**
         * @brief 上写锁
         */
        void lock() {
            if(!m_locked) {
                m_mutex.wrlock();
                m_locked = true;
            }
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            if(m_locked) {
                m_mutex.unlock();
                m_locked = false;
            }
        }
    private:
        /// Mutex
        T& m_mutex;
        /// 是否已上锁
        bool m_locked;
    };   
    
    //互斥量（锁）
    class Mutex : Noncopyable{    
        public:
            typedef ScopedLockImpl<Mutex> Lock;

            Mutex(){
                pthread_mutex_init(&m_mutex, nullptr);
            }

            ~Mutex(){
                pthread_mutex_destroy(&m_mutex);
            }

            void lock() {
                pthread_mutex_lock(&m_mutex);
            }

            void unlock() {
                pthread_mutex_unlock(&m_mutex);
            }
        private:
            //锁
            pthread_mutex_t m_mutex;
    };
    
    /**
     * @brief 空锁(用于对比不加锁的效果)
     */
    class NullMutex : Noncopyable {
        public:
        /// 局部锁
        typedef ScopedLockImpl<NullMutex> Lock;

        /**
        * @brief 构造函数
        */
        NullMutex() {}

        /**
        * @brief 析构函数
        */
        ~NullMutex() {}

        /**
        * @brief 加锁
        */
        void lock() {}

        /**
        * @brief 解锁
        */
        void unlock() {}
    };
    
    //读写锁
    class RWMutex : Noncopyable{
        public:

            /// 局部读锁
            typedef ReadScopedLockImpl<RWMutex> ReadLock;

            /// 局部写锁
            typedef WriteScopedLockImpl<RWMutex> WriteLock;

            /**
             * @brief 构造函数
             */
            RWMutex() {
                pthread_rwlock_init(&m_lock, nullptr);
            }
            
            /**
             * @brief 析构函数
             */
            ~RWMutex() {
                pthread_rwlock_destroy(&m_lock);
            }

            /**
             * @brief 上读锁
             */
            void rdlock() {
                pthread_rwlock_rdlock(&m_lock);
            }

            /**
             * @brief 上写锁
             */
            void wrlock() {
                pthread_rwlock_wrlock(&m_lock);
            }

            /**
             * @brief 解锁
             */
            void unlock() {
                pthread_rwlock_unlock(&m_lock);
            }

        private:
            pthread_rwlock_t m_lock;

    };

    /**
     * @brief 空读写锁(用于调试)
     */
    class NullRWMutex : Noncopyable {
    public:
            /// 局部读锁
            typedef ReadScopedLockImpl<NullMutex> ReadLock;
            /// 局部写锁
            typedef WriteScopedLockImpl<NullMutex> WriteLock;

            /**
            * @brief 构造函数
            */
            NullRWMutex() {}
            /**
            * @brief 析构函数
            */
            ~NullRWMutex() {}

            /**
            * @brief 上读锁
            */
            void rdlock() {}

            /**
            * @brief 上写锁
            */
            void wrlock() {}
            /**
            * @brief 解锁
            */
            void unlock() {}
    };

    /**
     * @brief 自旋锁
     */
    class Spinlock : Noncopyable {
    public:
        /// 局部锁
        typedef ScopedLockImpl<Spinlock> Lock;

        /**
         * @brief 构造函数
         */
        Spinlock() {
            pthread_spin_init(&m_mutex, 0);
        }

        /**
         * @brief 析构函数
         */
        ~Spinlock() {
            pthread_spin_destroy(&m_mutex);
        }

        /**
         * @brief 上锁
         */
        void lock() {
            pthread_spin_lock(&m_mutex);
        }

        /**
         * @brief 解锁
         */
        void unlock() {
            pthread_spin_unlock(&m_mutex);
        }
    private:
        /// 自旋锁
        pthread_spinlock_t m_mutex;
    };   

    /**
     * @brief 原子锁
     */
    class CASLock : Noncopyable{
    public:
        /// 局部锁
        typedef ScopedLockImpl<CASLock> Lock;

        /**
        * @brief 构造函数
        */
        CASLock() {
            m_mutex.clear();
        }

        /**
        * @brief 析构函数
        */
        ~CASLock() {
        }

        /**
        * @brief 上锁
        */
        void lock() {
            while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
        }

        /**
        * @brief 解锁
        */
        void unlock() {
            std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
        }
    private:
        /// 原子状态
        volatile std::atomic_flag m_mutex;
    };

    
    
    class Thread{
        public:
            //线程智能指针类型
            typedef std::shared_ptr<Thread> ptr;

            Thread(std::function<void()> cb, const std::string& name);

            /**
             * @brief 析构函数
             */
            ~Thread();

            /**
             * @brief 线程ID
             */
            pid_t getId() const { return m_id;}

            /**
             * @brief 线程名称
             */
            const std::string& getName() const { return m_name;}

            /**
             * @brief 等待线程执行完成
             */
            void join();

            /**
             * @brief 获取当前的线程指针
             */
            static Thread* GetThis();

            /**
             * @brief 获取当前的线程名称
             */
            static const std::string& GetName();

            /**
             * @brief 设置当前线程名称
             * @param[in] name 线程名称
             */
            static void SetName(const std::string& name);

        private:
            static void* run(void* arg);

            Thread(const Thread& thead) = delete;
            Thread(const Thread&&) = delete;
            Thread& operator=(const Thread&) = delete;

        private:
            pid_t m_id = -1;
            pthread_t m_thread = 0;
            std::string m_name;

            // 线程执行函数
            std::function<void()> m_cb;
            Semaphore m_semaphore;
        
    };


}