
#pragma once
#include "scheduler.h"
#include "timer.h"

namespace frb{

class IOManager : public Scheduler, public TimerManager{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event{
        NONE = 0x0,

        READ = 0x1,

        WRITE = 0x4,
    };

private:
    /**
     *  @brief 
    */
    struct FdContext {
        typedef Mutex MutexType;

        struct EventContext {
            /// 事件执行的调度器
            Scheduler* scheduler = nullptr;
            /// 事件协程
            Fiber::ptr fiber;
            /// 事件的回调函数
            std::function<void()> cb;   
        };

        EventContext& getContext(Event event);

        void resetContext(EventContext& ctx);
        
        /**
         * @brief 触发事件
         * @details 根据事件类型调用对应上下文结构中的调度器去调度回调协程或回调函数
         * @param[in] event 事件类型
         */
        void triggerEvent(Event event);

        int fd = 0;
        //读事件
        EventContext read;
        //写事件
        EventContext write;

        /// 当前的事件类型 【000 none、001 READ、100 WRITE、110 READ and WRITE】
        Event events = NONE;

        MutexType mutex;

    };


public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");

    ~IOManager();

    /**
     * @brief 添加事件
     * @param[in] fd socket句柄
     * @param[in] event 事件类型
     * @param[in] cb 事件回调函数
     * @return 添加成功返回0,失败返回-1
     */
    int addEvent(int fd, Event event, std::function<void()> = nullptr);

    /**
     * @brief 删除事件
     * @param[in] fd socket句柄
     * @param[in] event 事件类型
     * @attention 不会触发事件
     */
    bool delEvent(int fd, Event event);

    /**
     * @brief 取消事件
     * @param[in] fd socket句柄
     * @param[in] event 事件类型
     * @attention 如果事件存在则触发事件
     */
    bool cancelEvent(int fd, Event event);

    /**
     * @brief 取消所有事件
     * @param[in] fd socket句柄
     */
    bool cancelAll(int fd);

    /**
     * @brief 返回当前的IOManager
     */
    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertedAtFront() override;

    void contextResize(size_t size);

    /**
     * @brief 判断是否可以停止
     * @param[out] timeout 最近要出发的定时器事件间隔
     * @return 返回是否可以停止
     */
    bool stopping(uint64_t& timeout);



private:
    ///
    RWMutexType m_mutex;

    /// epoll 内核事件表
    int m_epfd = 0;

    /// pipe
    int m_tickleFds[2];

    /// 当前等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount = {0};

    /**
     *  @brief socket事件上下文的容器，保存每个socket上发生的事件
     * 
    */
    std::vector<FdContext*> m_fdContexts;
};


}