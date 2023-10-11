#include <iostream>
#include <string>
#include "../include/log.h"
#include "../include/utils.h"


int main(int argc, char** argv) {
    frb::Logger::ptr logger(new frb::Logger);
    logger->setLevel(frb::LogLevel::ERROR);

    frb::StdoutLogAppender::ptr std_appender(new frb::StdoutLogAppender);
    std_appender->setLevel(frb::LogLevel::ERROR);
    logger->addAppender(std_appender);

    frb::FileLogAppender::ptr file_appender(new frb::FileLogAppender("./log.txt"));
    frb::LogFormatter::ptr fmt(new    frb::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(frb::LogLevel::ERROR);

    logger->addAppender(file_appender);

    const std::string treadname("tread");

    frb::LogEvent::ptr event(new  frb::LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 0,  1,  2, time(0), treadname));
    event->getSS() << "hello  frb log";
    logger->log(frb::LogLevel::DEBUG, event);

    auto l = frb::LoggerMgr::GetInstance()->getLogger("xx");
    LOG_ERROR(l, "this is error");
    return 0;
}