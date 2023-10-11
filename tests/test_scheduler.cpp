#include "../include/scheduler.h"

static frb::Logger::ptr g_logger = GET_LOG_ROOT;

void test_fiber(){
    static int s_count = 3;
    LOG_INFO_STREAM(g_logger) << "test in fiber s_count=" << s_count;

    sleep(1);
    if(--s_count >= 0) {
        frb::Scheduler::GetThis()->schedule(&test_fiber);
    }
}
int main(){

    LOG_INFO_STREAM(g_logger) << "main";

    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/bin/conf/log.yaml");
    frb::Config::LoadFromYaml(root);
    
    frb::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    sc.schedule(&test_fiber);
    LOG_INFO_STREAM(g_logger) << "schedule";
    sc.stop();
    LOG_INFO_STREAM(g_logger) << "end";
}