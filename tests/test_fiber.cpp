
#include "../include/fiber.h"
#include "../include/log.h"
#include "../include/config.h"

frb::Logger::ptr g_logger = GET_LOG_ROOT;


void run_in_fiber(){
    LOG_INFO_STREAM(g_logger) <<" run in fiber(before swapout)";
    frb::Fiber::YieldToHold();

    LOG_INFO_STREAM(g_logger) <<" run in fiber(after swapout)";
    frb::Fiber::YieldToHold();

    LOG_INFO_STREAM(g_logger) <<" run in fiber(end)";
}

void test_fiber(){
    
        frb::Fiber::GetThis();
        frb::Fiber::ptr fb = std::make_shared<frb::Fiber>(run_in_fiber);
        LOG_INFO_STREAM(g_logger) <<" run in main(fiber construct)";
        fb->swapIn();
        LOG_INFO_STREAM(g_logger) <<" run in main(after first swapin)";
        fb->swapIn();
        LOG_INFO_STREAM(g_logger) <<" run in main(after second swapin)";
        fb->swapIn();
}
int main() {
    
    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/bin/conf/log.yaml");
    frb::Config::LoadFromYaml(root);

    std::vector<frb::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i) {
        thrs.push_back(frb::Thread::ptr(
                    new frb::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i : thrs) {
        i->join();
    }

    //出作用域后fb没有被析构（并且下面的语句没有执行[解决方法，当前协程的mainfunc执行完后切回主协程]），主协程正常析构
    //为什么没有被析构：
    LOG_INFO_STREAM(g_logger) <<" run in main(end)";


}