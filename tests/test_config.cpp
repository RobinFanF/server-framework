
#include "../include/config.h"
#include "../include/log.h"
#include <yaml-cpp/yaml.h>

frb::ConfigVar<int>::ptr g_int_value_config = frb::Config::Lookup("system.port", (int)8080, "system port");

frb::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config =
    frb::Config::Lookup("system.int_vec", std::vector<int>{98,45}, "system int vec");

void test_loadconf(){
    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/conf/log.yaml");
    
    std::cout<< root <<std::endl;
}


void test_config(){

    auto logger = frb::LoggerMgr::GetInstance()->getLogger("xx");
    logger->setLevel(frb::LogLevel::ERROR);


    //LOG_ERROR(logger, "this is error");

    //std::cout<< g_int_value_config->getValue() << std::endl;
    frb::LogEvent::ptr event1(new  frb::LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 0,  1,  2, time(0), g_int_value_config->toString()));
    
    g_int_value_config->addListener([](const int& old_value, const int& new_value)->void{
        std::cout<<"old value :"<< old_value <<" new value:"<<new_value<<std::endl;
    });

    logger->log(frb::LogLevel::INFO, event1);

    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/bin/conf/log.yaml");
    frb::Config::LoadFromYaml(root); 

    frb::LogEvent::ptr event2(new  frb::LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 0,  1,  2, time(0), g_int_value_config->toString()));
    logger->log(frb::LogLevel::INFO, event2);


    // LOG_INFO(logger, g_int_value_config->toString());
    // LOG_INFO(GET_LOG_ROOT(), g_int_value_config->toString());  //log有问题，有时候会没有输出
}

void test_complex_config(){
    // frb::StdoutLogAppender::ptr std_appender(new frb::StdoutLogAppender);
    // std_appender->setLevel(frb::LogLevel::ERROR);
    
    auto logger = frb::LoggerMgr::GetInstance()->getLogger("xx");
    logger->setLevel(frb::LogLevel::ERROR);
    //logger->addAppender(std_appender);

    //LOG_ERROR(logger, "this is error");

    //std::cout<< g_int_value_config->getValue() << std::endl;
    frb::LogEvent::ptr event1(new  frb::LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 0,  1,  2, time(0), g_int_vec_value_config->toString()));
    

    logger->log(frb::LogLevel::INFO, event1);

    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/bin/conf/log.yaml");
    frb::Config::LoadFromYaml(root);

    frb::LogEvent::ptr event2(new  frb::LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 0,  1,  2, time(0), g_int_vec_value_config->toString()));
    logger->log(frb::LogLevel::INFO, event2);
}


void test_log(){
    //日志器配置参数的类型是set<LogDefine>（初始化为空）
    // logs:


    static frb::Logger::ptr system_log = GET_LOG_NAME("system");
    std::cout<< frb::LoggerMgr::GetInstance()->toYamlString()<< std::endl;
    LOG_INFO(GET_LOG_ROOT, "root logger format before load from yaml file");
    std::cout << "==============================================================================" << std::endl;
    YAML::Node root = YAML::LoadFile("/home/bing/mycode2022/server-framework/conf/log.yaml");

    //在更新参数得实际值时，反序列化 字节流到 set<logDefine>
    frb::Config::LoadFromYaml(root); 
    
    std::cout<< frb::LoggerMgr::GetInstance()->toYamlString()<< std::endl;
    std::cout << "==============================================================================" << std::endl;
    LOG_INFO(GET_LOG_ROOT, "root logger format after load from yaml file");
    std::cout<<std::endl;
    
    

}


int main(){


    //test_config();
    //test_loadconf();
    //test_complex_config();

    //std::cout<<"main"<<std::endl;
    test_log();


}