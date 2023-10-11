#include "../include/timer.h"
#include "../include/utils.h"

namespace frb{

//Timer的比较方法，set(红黑树原理??)需要用到
bool Timer::Comparator::operator()(const Timer::ptr& lhs
                        ,const Timer::ptr& rhs) const {
    if(!lhs && !rhs) {
        return false;
    }
    if(!lhs) {
        return true;
    }
    if(!rhs) {
        return false;
    }
    if(lhs->m_next < rhs->m_next) {
        return true;
    }
    if(rhs->m_next < lhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb,
             bool recurring, TimerManager* manager)
    :m_recurring(recurring)
    ,m_ms(ms)
    ,m_cb(cb)
    ,m_manager(manager) {
    m_next = frb::GetCurrentMS() + m_ms;
}

Timer::Timer(uint64_t next)
    :m_next(next) {
}

//Timer的m_cb为空代表已经cancel过了
bool Timer::cancel() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    
    if(m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);//删除指向timer的智能指针
        return true;
    }
    return false; 
}

//从时间堆中删除后，刷新执行时间重新加入时间堆
bool Timer::refresh(){
    
    if(!m_cb){
        return false;
    }

    auto it = m_manager->m_timers.find(shared_from_this());

    //和 !m_cb 是否重复？？？
    if(it == m_manager->m_timers.end()) {
        return false;
    }

    m_manager->m_timers.erase(it);
    m_next = frb::GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}


// from_now :true 新的执行时间是现在的时间加上间隔时间
//          :false 新的执行时间是之前的时间（timer构造的时候）加上间隔时间
bool Timer::reset(uint64_t ms, bool from_now) {
    
    //相当于没变
    if(ms == m_ms && !from_now) {
        return true;
    }
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if(from_now) {
        start = frb::GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;

    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

TimerManager::TimerManager() {
    m_previouseTime = frb::GetCurrentMS();
}

TimerManager::~TimerManager() {
}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb
                                  ,bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb
                                    ,std::weak_ptr<void> weak_cond
                                    ,bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

//返回多少时间后最小的定时器到期

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    
    //如果没有定时器
    if(m_timers.empty()) {
        return ~0ull;
    }

    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = frb::GetCurrentMS();


    if(now_ms >= next->m_next) {
        //有过期的定时器
        return 0;
    } else {
        //
        return next->m_next - now_ms;
    }
}

//每次超时之后除了要检查有没有要触发的定时器，还顺便检查一下系统时间有没有被往回调。
void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms = frb::GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if(m_timers.empty()) {
            return;
        }
    }
    RWMutexType::WriteLock lock(m_mutex);
    if(m_timers.empty()) {
        return;
    }

    bool rollover = detectClockRollover(now_ms);

    //没有回调，但是没有要触发的定时器
    if(!rollover && (*m_timers.begin())->m_next > now_ms) {
        return;
    }
    

    Timer::ptr now_timer(new Timer(now_ms));

    //回调了，把所有定时器都触发
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while(it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for(auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if(timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}
//如果插入的timer的唤醒时间最小，则需要通知epoll_wait修改超时时间
void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock){
    auto it = m_timers.insert(val).first;

    bool at_front = it == m_timers.begin();

    lock.unlock();

    if(at_front){
        onTimerInsertedAtFront();
    }

}


//如果系统时间往回调了1个小时以上，那就触发全部定时器。
bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if(now_ms < m_previouseTime &&
            now_ms < (m_previouseTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previouseTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

}