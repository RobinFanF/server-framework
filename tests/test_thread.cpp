
#include <unistd.h>
#include "../include/log.h"
#include "../include/thread.h"
#include "../include/config.h"

frb::Logger::ptr g_logger = GET_LOG_ROOT;
frb::Logger::ptr sys_logger = GET_LOG_NAME("system");

static int count = 0;

frb::Mutex s_mutex;

void fun1() {
    // LOG_INFO_STREAM(g_logger) << "name: " << frb::Thread::GetName()
    //                          << " this.name: " << frb::Thread::GetThis()->getName()
    //                          << " id: " << frb::GetThreadId()
    //                          << " this.id: " << frb::Thread::GetThis()->getId();

    for(int i = 0; i < 1000000; ++i) {  
        frb::Mutex::Lock lock(s_mutex);    
        ++count;
        
    }
}

void fun2() {
    while(true) {
        LOG_INFO_STREAM(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void fun3() {
    while(true) {
        LOG_INFO_STREAM(sys_logger) << "========================================";
    }
}

int main(int argc, char** argv) {
    LOG_INFO_STREAM(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/bin/conf/log.yaml");
    frb::Config::LoadFromYaml(root);

    std::vector<frb::Thread::ptr> thrs;
    for(int i = 0; i < 1; ++i) {
        frb::Thread::ptr thr1(new frb::Thread(&fun2, "name_" + std::to_string(i * 2)));
        frb::Thread::ptr thr2(new frb::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
        thrs.push_back(thr1);
        thrs.push_back(thr2);
       
    }

    for(size_t i = 0; i < thrs.size(); ++i) {
        thrs[i]->join();
    }
    LOG_INFO_STREAM(g_logger) << "thread test end";
    LOG_INFO_STREAM(g_logger) << "count=" << count;

    return 0;
}