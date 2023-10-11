#pragma once 

#include "fiber.h"
#include "thread.h"

namespace frb{

class Scheduler{
    public:
        typedef std::shared_ptr<Scheduler> ptr;
        typedef Mutex MutexType;
        
        Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "scheduler");
        
        virtual ~Scheduler();

        const std::string& getName() const {return m_name;}

        //返回当前协程调度器

        static Scheduler* GetThis();

        //返回当前的调度协程
        static Fiber* GetMainFiber();

        /**
         * @brief 启动协程调度器
         */
        void start();

        /**
         *  @brief 停止协程调度器
        */
        void stop();
        

        //把任务加入协程调度器
        // template<typename Executable>
        // void schedule(Executable&& exec, int thread = -1){
            
        //     bool need_tickle = false;
        //     {
        //         MutexType::Lock lock(m_mutex);
        //         need_tickle = scheduleNoLock(std::forward<Executable>(exec), thread);
        //     }

        //     if(need_tickle){
        //         tickle();
        //     }
        // }

        //把任务加入协程调度器
        template<typename Executable>
        void schedule(Executable exec, int thread = -1){
            bool need_tickle = false;
            {
                MutexType::Lock lock(m_mutex);
                need_tickle = scheduleNoLock(exec, thread);
            }

            if(need_tickle){
                tickle();
            }
        }

        template<typename InputIterator>
        void schedule(InputIterator begin, InputIterator end){
            bool need_tickle = false;

            {
                MutexType::Lock lock(m_mutex);
                while(begin != end){
                    need_tickle = scheduleNoLock(*begin, -1) || need_tickle;
                    ++begin;
                }

                if(need_tickle){
                    tickle();
                }
            }
        }
    protected:

        /** 
         *  @brief 通知协程调度器有任务了
        */
        virtual void tickle();

        /**
         * @brief 协程调度函数
         */
        void run();

        /**
         * @brief 返回是否可以停止
         */
        virtual bool stopping();  

        /**
         * @brief 协程无任务可调度时执行idle协程
         */
        virtual void idle();   

        /**
         * @brief 设置当前的协程调度器
         */
        void setThis(); 

        /**
         * @brief 是否有空闲线程
         */
        bool hasIdleThreads() { return m_idleThreadCount > 0;}          

    private:
        template<typename Executable>
        bool scheduleNoLock(Executable exec, int thread) {
            bool need_tickle = m_fibers.empty();
            
            //创建一个Task
            Task task(exec, thread);

            //创建的任务实例存在有效的
            if(task.fiber || task.cb){
                m_fibers.push_back(task);
            }

            //如果调度任务队列为空，need_ticle
            return need_tickle;
        }
    private:
        //一个调度任务可以是协程和函数
        struct Task {
            typedef std::shared_ptr<Task> ptr;
            using TaskFunc = std::function<void()>;
            //期望的线程id，(-1代表可以被任何线程调度）
            int thread;
            
            Fiber::ptr fiber;
            TaskFunc cb;

            // //使用移动语义（这样做的好处呢）
            // Task(Fiber::ptr f, int thr) 
            // : fiber(std::move(f))
            // , thread(thr){
            // }

            Task(TaskFunc f, int thr)
            : thread(thr)
            , cb(f){
            }

            Task(std::function<void()>* f, int thr)
                :thread(thr) {
                cb.swap(*f);
            }

            Task(Fiber::ptr f, int thr) 
            : fiber(f)
            , thread(thr){
            }


            //这是什么
            Task(Fiber::ptr* f, int thr)
                :thread(thr) {
                fiber.swap(*f);
            }

            Task()
            : thread(-1){
            }

            void reset(){
                fiber = nullptr;
                cb = nullptr;
                thread = -1;
            }

        };

    private:
        MutexType m_mutex;
        // 线程池
        std::vector<Thread::ptr> m_threads;
        // 待执行的协程队列
        std::list<Task> m_fibers;
        // use_caller为true时有效,
        //caller线程创建的主协程，用于调度协程
        Fiber::ptr m_rootFiber;
        /// 协程调度器名称
        std::string m_name;
    
    protected:
        /// 仅在use_caller 为true时，记录caller线程的id
        int m_rootThread = 0;
        /// 线程id数组
        std::vector<int> m_threadIds;
        /// 线程数量
        size_t m_threadCount = 0;
        /// 工作线程数量
        std::atomic<size_t> m_activeThreadCount = {0};
        /// 空闲线程数量
        std::atomic<size_t> m_idleThreadCount = {0};
        /// 是否正在停止
        bool m_stopping = false;
        /// 是否自动停止
        bool m_autoStop = false;
    };
}