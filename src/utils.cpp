
#include <execinfo.h>
#include <sys/time.h>
#include "../include/fiber.h"
#include "../include/utils.h"
#include "../include/log.h"



namespace frb{

    static frb::Logger::ptr g_logger = GET_LOG_NAME("system");

    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberId() {
        return frb::Fiber::GetFiberId();
    }

    uint64_t GetCurrentMS(){
        timeval time;

        gettimeofday(&time, 0);

        return time.tv_sec * 1000ul + time.tv_usec / 1000;
    }

    /**
     * @brief 获取当前的调用栈
     * @param[out] bt 保存调用栈
     * @param[in] size 最多返回层数
     * @param[in] skip 跳过栈顶的层数
     */
    void Backtrace(std::vector<std::string>& bt, int size, int skip){
        void** buffer =  (void**)malloc(sizeof(void*) * size);

        size_t nptrs = backtrace(buffer, size);

        char** strings = backtrace_symbols(buffer, nptrs);

        if(strings == NULL) {
            LOG_ERROR_STREAM(g_logger) << "backtrace_synbols error";
            return;
        }

        for(size_t i = skip; i < nptrs; ++i){
            bt.push_back(strings[i]);
        }

        free(strings);
        free(buffer);


    }

    std::string BacktraceToString(int size, int skip , const std::string& prefix){
        std::vector<std::string> bt;

        Backtrace(bt, size, skip);

        std::stringstream ss;
        for(size_t i = 0; i < bt.size(); ++i) {
            ss << prefix << bt[i] << std::endl;
        }
        return ss.str();
    }

}