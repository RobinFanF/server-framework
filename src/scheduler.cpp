
#include "../include/scheduler.h"
#include "../include/hook.h"


namespace frb{

    static frb::Logger::ptr g_logger = GET_LOG_NAME("system");
    //当前调度器
    static thread_local Scheduler* t_scheduler = nullptr;
    //当前调度协程
    static thread_local Fiber* t_scheduler_fiber = nullptr;

    Scheduler::Scheduler(size_t threads , bool use_caller , const std::string& name )
        : m_name(name) {
            ASSERT(threads > 0);

            if(use_caller){
                frb::Fiber::GetThis();
                --threads;

                ASSERT(GetThis() == nullptr);
                t_scheduler = this;

                
                //在caller线程里，调度协程并不是caller线程的主协程，而是相当于caller线程的子协程
                m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
                frb::Thread::SetName(m_name);

                t_scheduler_fiber = m_rootFiber.get();
                m_rootThread = frb::GetThreadId();
                m_threadIds.push_back(m_rootThread);
            } else {
                m_rootThread = -1;
            }

            m_threadCount = threads;
        }
    
    
    Scheduler::~Scheduler(){
        ASSERT(m_stopping);
        if(GetThis() == this) {
            t_scheduler = nullptr;
        }
    }

    Scheduler* Scheduler::GetThis(){
        return t_scheduler;
    }

    
    Fiber* Scheduler::GetMainFiber(){
        return t_scheduler_fiber;
    }

    void Scheduler::start(){
        MutexType::Lock lock(m_mutex);

        if(m_stopping){
            return;
        }
        ASSERT(m_threads.empty());

        m_threads.resize(m_threadCount);
        for(size_t i = 0; i < m_threadCount; ++i){
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                                , m_name + "_" + std::to_string(i)));
            m_threadIds.push_back(m_threads[i]->getId());
        }
    }

    void Scheduler::stop(){
        m_autoStop = true;

        //只有caller线程参与调度
        if(m_rootFiber
                && m_threadCount == 0
                && (m_rootFiber->getState() == Fiber::TERM
                    || m_rootFiber->getState() == Fiber::INIT)) {
            LOG_INFO_STREAM(g_logger) << this << " stopped";
            m_stopping = true;

            //没事干了，直接停止，不用开始调度了
            if(stopping()) {
                return;
            }
        }
        
        //bool exit_on_this_fiber = false;
        if(m_rootThread != -1) {
            ASSERT(GetThis() == this);
        } else {
            ASSERT(GetThis() != this);
        }

        m_stopping = true;
        for(size_t i = 0; i < m_threadCount; ++i) {
            tickle();
        }

        if(m_rootFiber) {
            tickle();
        }

        //caller的调度协程
        if(m_rootFiber) {
            if(!stopping()) {
                m_rootFiber->call();
            }
        }
        

        //回收子线程
        std::vector<Thread::ptr> thrs;
        {
            MutexType::Lock lock(m_mutex);
            thrs.swap(m_threads);
        }


        
        for(auto& i : thrs) {
            i->join();
        }
    }


    void Scheduler::tickle(){
        LOG_INFO_STREAM(g_logger) << "tickle";
    }

    /**
     *  @brief 线程开始调度，即找到一个合适的任务开始执行
    */
    void Scheduler::run(){
        setThis();
        set_hook_enable(true);

        //非caller线程
        if(frb::GetThreadId() != m_rootThread) {
            t_scheduler_fiber = Fiber::GetThis().get();
        }

        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
        Fiber::ptr cb_fiber;

        Task task;

        while(true){
            task.reset();
            bool tickle_me = false;
            bool is_active = false;
            {
                MutexType::Lock lock(m_mutex);
                auto it = m_fibers.begin();
                while(it != m_fibers.end()){
                    //协程有期望的线程号，但不是当前线程
                    if(it->thread != -1 &&
                        it->thread != frb::GetThreadId()) {
                            ++it;
                            tickle_me = true;
                            continue;
                        }

                    ASSERT(it->fiber || it->cb);

                    //如果是协程，但处于执行态
                    if(it->fiber && it->fiber->getState() == Fiber::EXEC) {
                        ++it;
                        continue;
                    }

                    task = *it;       
                    m_fibers.erase(it++);
                    ++m_activeThreadCount;
                    is_active = true;
                
                    break;
                }

    
                tickle_me |= (it != m_fibers.end());
            }

            if(tickle_me){
                tickle();
            }

            //三种情况
            //1. task是一个协程
            //2. task是一个函数
            //3. 没有要处理的任务，则执行idel协程
            if(task.fiber && (
                task.fiber->getState() != Fiber::TERM
                && task.fiber->getState() != Fiber::EXCEPT)){

                task.fiber->swapIn();
                --m_activeThreadCount;

                if(task.fiber->getState() == Fiber::READY){
                    schedule(task.fiber);
                } else if(task.fiber->getState() != Fiber::TERM
                    && task.fiber->getState() != Fiber::EXCEPT) {
                    task.fiber->m_state = Fiber::HOLD;
                } 

                task.reset();
            } else if(task.cb)  {
                if(cb_fiber){
                    cb_fiber->reset(task.cb);
                } else {
                    cb_fiber.reset(new Fiber(task.cb));
                }

                task.reset();
                cb_fiber->swapIn();
                --m_activeThreadCount;
                if(cb_fiber->getState() == Fiber::READY) {
                    schedule(cb_fiber);
                    cb_fiber.reset();
                } else if(cb_fiber->getState() == Fiber::EXCEPT
                        || cb_fiber->getState() == Fiber::TERM) {
                    cb_fiber->reset(nullptr);
                } else {
                    cb_fiber->m_state = Fiber::HOLD;
                    cb_fiber.reset();
                } 
            } else {
                
                //这是什么
                if(is_active) {
                    --m_activeThreadCount;
                    continue;
                }

                if(idle_fiber->getState() == Fiber::TERM) {
                    LOG_INFO_STREAM(g_logger) << "idle fiber term";
                    break;
                }
                ++m_idleThreadCount;
                idle_fiber->swapIn();
                --m_idleThreadCount;

                if(idle_fiber->getState() != Fiber::TERM
                                    && idle_fiber->getState() != Fiber::EXCEPT) {
                    idle_fiber->m_state = Fiber::HOLD;
                }
            }
        }
    
    }


    bool Scheduler::stopping(){
        MutexType::Lock lock(m_mutex);
        return m_autoStop && m_stopping
               && m_fibers.empty() && m_activeThreadCount == 0;
    }


    void Scheduler::idle()  {
        LOG_INFO_STREAM(g_logger) << "idle";
        while(!stopping()) {
            frb::Fiber::YieldToHold();
        }
    }

    //
    void Scheduler::setThis(){
        t_scheduler = this;
    }


}