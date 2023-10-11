#include "../include/fiber.h"
#include "../include/scheduler.h"

namespace frb{

    static Logger::ptr g_logger = GET_LOG_NAME("system");
    static std::atomic<uint64_t> s_fiber_id {0};
    static std::atomic<uint64_t> s_fiber_count {0};

    static thread_local Fiber* t_fiber = nullptr;               //记录当前协程
    static thread_local Fiber::ptr t_threadFiber = nullptr;     //记录主协程

    //可配置的协程栈的大小
    static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
        Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size"); 
    

    //用于分配协程栈
    class MallocStackAllocator {
    public:
        static void* Alloc(size_t size) {
            return malloc(size);
        }

        static void Dealloc(void* vp, size_t size) {
            return free(vp);
        }
    };

    using StackAllocator = MallocStackAllocator;   
    //线程的主协程
    //主协程只负责创建其它协程，不执行函数，因此不需要栈
    Fiber::Fiber(){
        m_state = EXEC;
        SetThis(this);

        if(getcontext(&m_ctx)){
            ASSERT2(false, "getcontext"); 
        }

        //线程的主协程的id始终为0
        // ++s_fiber_id;
        // m_id = s_fiber_id;
        ++s_fiber_count;

        LOG_DEBUG_STREAM(g_logger) << "Fiber::Fiber main";

    }

    /**
     * @brief 构造函数
     * @param[in] cb 协程执行的函数
     * @param[in] stacksize 协程栈大小
     * @param[in] use_caller 是否在MainFiber上调度
     */
    Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
        : m_id(++s_fiber_id)
        , m_cb(cb) {
        ++s_fiber_count;
        m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
        m_stack = StackAllocator::Alloc(m_stacksize);
        if(getcontext(&m_ctx)) {
            ASSERT2(false, "getcontext");
        }
        

        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;


        //修改由getcontext获取到的上下文指针ucp，将其与一个函数func进行绑定，支持指定func运行时的参数
        //调用setcontext或swapcontext激活ucp时，func就会被运行
        //初始化一个ucontext_t,arg2指明了该context的入口函数
        //arg3：入口函数的参数个数

        if(use_caller){
            makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
        } else {
            makecontext(&m_ctx, &Fiber::MainFunc, 0);
        }
        

        LOG_DEBUG_STREAM(g_logger) << "Fiber::Fiber fiber id = " << m_id;
    }

    /**
     * @brief 析构函数
     */
    Fiber::~Fiber(){
    --s_fiber_count;

    // 区分主协程 和 其它协程
    if(m_stack) {
        ASSERT(m_state == TERM
                || m_state == EXCEPT
                || m_state == INIT);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        ASSERT(!m_cb);
        ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this) {
            SetThis(nullptr);
        }
    }
    LOG_DEBUG_STREAM(g_logger) << "Fiber::~Fiber id=" << m_id
                              << " total=" << s_fiber_count;
    }


    uint64_t Fiber::GetFiberId() {
        if(t_fiber) {
            return t_fiber->getId();
        }
        return 0;
    }
    //设置当前协程
    void Fiber::SetThis(Fiber* f) {
        t_fiber = f;
    }

    //（在协程还未释放之前可以重置协程）
    void Fiber::reset(std::function<void()> cb){
        //主协程不能重置
        ASSERT(m_stack);
        ASSERT(m_state == TERM
            || m_state == EXCEPT
            || m_state == INIT);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT; 

    }
    
    
    void Fiber::call(){
        SetThis(this);
        m_state = EXEC;
        if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
            ASSERT2(false, "swapcontext");
        }
    }
    //切换到当前协程执行(从主协程切换到其它协程)
    void Fiber::swapIn() {
        SetThis(this);

        //不能切换正在执行的协程
        ASSERT(m_state != EXEC);
        m_state = EXEC;
        if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
            ASSERT2(false, "swapcontext");
        }
    }

    void Fiber::back(){
        SetThis(t_threadFiber.get());
        if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
            ASSERT2(false, "swapcontext");
        }
    }

    //切换到后台
    void Fiber::swapOut() {
        SetThis(this);
        if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)) {
            ASSERT2(false, "swapcontext");
        }
    } 

    //返回当前协程
    //如果当前没有协程（说明主协程都没有，则创建一个主协程）
    Fiber::ptr Fiber::GetThis() {
        if(t_fiber) {
            return t_fiber->shared_from_this();
        }
        Fiber::ptr main_fiber(new Fiber);
        ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
        return t_fiber->shared_from_this();
    }  

    //(当前正在执行的协程)协程切换到后台，并且设置为Ready状态
    void Fiber::YieldToReady() {
        Fiber::ptr cur = GetThis();
        ASSERT(cur->m_state == EXEC);
        cur->m_state = READY;
        cur->swapOut();
    }

    //协程切换到后台，并且设置为Hold状态
    void Fiber::YieldToHold() {
        Fiber::ptr cur = GetThis();
        ASSERT(cur->m_state == EXEC);
        cur->m_state = HOLD;
        cur->swapOut();
    }

    //总协程数
    uint64_t Fiber::TotalFibers() {
        return s_fiber_count;
    }    

    void Fiber::MainFunc() {

        Fiber::ptr cur = GetThis();
        ASSERT(cur);
        try {
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        } catch (std::exception& ex) {
            cur->m_state = EXCEPT;
            LOG_ERROR_STREAM(g_logger) << "Fiber Except: " << ex.what()
                << " fiber_id=" << cur->getId()
                << std::endl
                << frb::BacktraceToString();
        } catch (...) {
            cur->m_state = EXCEPT;
            LOG_ERROR_STREAM(g_logger) << "Fiber Except"
                << " fiber_id=" << cur->getId()
                << std::endl
                << frb::BacktraceToString();
        }


        auto raw = cur.get();
        cur.reset();
        raw->swapOut();
    }

    void Fiber::CallerMainFunc(){
        LOG_INFO_STREAM(g_logger)<<" CallerMainFunc";
        Fiber::ptr cur = GetThis();
        ASSERT(cur);
        try {
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        } catch (std::exception& ex) {
            cur->m_state = EXCEPT;
            LOG_ERROR_STREAM(g_logger) << "Fiber Except: " << ex.what()
                << " fiber_id=" << cur->getId()
                << std::endl
                << frb::BacktraceToString();
        } catch (...) {
            cur->m_state = EXCEPT;
            LOG_ERROR_STREAM(g_logger) << "Fiber Except"
                << " fiber_id=" << cur->getId()
                << std::endl
                << frb::BacktraceToString();
        }

        LOG_INFO_STREAM(g_logger)<<" CallerMainFunc end";

        auto raw = cur.get();
        cur.reset();
        raw->back(); 
    }
}