#include "../include/thread.h"
#include "../include/log.h"
#include "../include/utils.h"

namespace frb{

    static thread_local Thread* t_thread = nullptr;
    static thread_local std::string t_thread_name = "UNKNOW";

    static frb::Logger::ptr g_logger = GET_LOG_NAME("system");


    Semaphore::Semaphore(uint32_t val){
        if(sem_init(&m_semaphore, 0, val)){
            throw std::logic_error("sem_init error");
        }
        

    }
    Semaphore::~Semaphore(){
        sem_destroy(&m_semaphore);
    }

    void Semaphore::wait(){
        if(sem_wait(&m_semaphore)){
            throw std::logic_error("sem_wait error");
        }
        
    }
    void Semaphore::notify(){
        if(sem_post(&m_semaphore)){
            throw std::logic_error("sem_post error");
        }
    }

    /**
    * @brief 获取当前的线程指针
    */
    Thread* Thread::GetThis(){
        return t_thread;
    }
    /**
     * @brief 获取当前的线程名称
     */
    const std::string& Thread::GetName(){
        return t_thread_name;
    }

    void Thread::SetName(const std::string& name){
        if(name.empty()) {
            return;
        }
        if(t_thread) {
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }

    Thread::Thread(std::function<void()> cb, const std::string& name)
      :m_cb(cb)
      ,m_name(name) {

        if(name.empty()) {
        m_name = "UNKNOW";
      }

      int rt = pthread_create(&m_thread, NULL, &Thread::run, this);

      if(rt){
        LOG_ERROR(g_logger, "pthread_create thread fail, rt=" + std::to_string(rt));
        throw std::logic_error("pthread_create error");
      }
      
      //等待子线程执行
      m_semaphore.wait();


    }
    

    Thread::~Thread(){
        if(m_thread) {
            pthread_detach(m_thread);//主线程与子线程分离，互不干扰
        }
    }


    void* Thread::run(void* arg){
        Thread* thread = (Thread*) arg;
        t_thread = thread;
        t_thread_name = thread->m_name;
        thread->m_id = frb::GetThreadId();
        pthread_setname_np(pthread_self(),thread->m_name.substr(0,15).c_str());


        std::function<void()> cb;
        cb.swap(thread->m_cb);

        //唤醒等待线程
        thread->m_semaphore.notify();

        cb();

        return 0;
    }


    void Thread::join(){
        if(m_thread){
            int rt = pthread_join(m_thread, nullptr); // 主线程阻塞等待子线程
            if(rt){
                LOG_ERROR(g_logger, "pthread_join thread fail, rt=" + std::to_string(rt));
                throw std::logic_error("pthread_join error");               
            }

            m_thread = 0;
        }
    }
}