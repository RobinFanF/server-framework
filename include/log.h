#pragma once

#include <string>
#include <list>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <functional>

#include "singleton.h"
#include "thread.h"
#include "utils.h"



#define GET_LOG_ROOT frb::LoggerMgr::GetInstance()->getRoot()
#define GET_LOG_NAME(name) frb::LoggerMgr::GetInstance()->getLogger(name)
#define GET_Manager frb::LoggerMgr::GetInstance()

#define MAKE_LOG_EVENT(logger, level, massage) \
    std::make_shared<frb::LogEvent>(logger, level, __FILE__, __LINE__, 0, frb::GetThreadId(), frb::GetFiberId(), time(0), frb::Thread::GetName())

#define LOG_LEVEL(logger, level, massage) \
    logger->log(level, MAKE_LOG_EVENT(logger, level, massage));
    
#define LOG_LEVEL_STREAM(logger, level) \ 
    if(logger->getLevel() >= level) \
        frb::LogEventWrap(MAKE_LOG_EVENT(logger, level, "线程名称")).getSS()
    

#define LOG_DEBUG(logger, massage) LOG_LEVEL(logger, frb::LogLevel::DEBUG, massage)
#define LOG_INFO(logger, massage) LOG_LEVEL(logger, frb::LogLevel::INFO, massage)
#define LOG_WARN(logger, massage) LOG_LEVEL(logger, frb::LogLevel::WARN, massage)
#define LOG_ERROR(logger, massage) LOG_LEVEL(logger, frb::LogLevel::ERROR, massage)
#define LOG_FATAL(logger, massage) LOG_LEVEL(logger, frb::LogLevel::FATAL, massage)

//使用流式方式将日志级别level的日志写入到logger
#define LOG_DEBUG_STREAM(logger) LOG_LEVEL_STREAM(logger, frb::LogLevel::DEBUG)
#define LOG_INFO_STREAM(logger) LOG_LEVEL_STREAM(logger, frb::LogLevel::INFO)
#define LOG_WARN_STREAM(logger) LOG_LEVEL_STREAM(logger, frb::LogLevel::WARN)
#define LOG_ERROR_STREAM(logger) LOG_LEVEL_STREAM(logger, frb::LogLevel::ERROR)
#define LOG_FATAL_STREAM(logger) LOG_LEVEL_STREAM(logger, frb::LogLevel::FATAL)




namespace frb{
    class Logger;
    class LogManager;
    typedef Singleton<LogManager> LoggerMgr;

    class LogLevel{
        public:
            enum Level{
                UNKNOW = 0,
                DEBUG = 1,
                INFO = 2,
                WARN = 3,
                ERROR = 4,
                FATAL = 5
            };

            static const char* ToString(Level level);

            static Level FromString(const std::string& str);

    };

    class LogEvent{
        public:
            typedef std::shared_ptr<LogEvent> ptr;

            LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name);    

            /**
            * @brief 返回文件名
            */
            const char* getFile() const { return m_file;}

            /**
             * @brief 返回行号
             */
            int32_t getLine() const { return m_line;}

            /**
             * @brief 返回耗时
             */
            uint32_t getElapse() const { return m_elapse;}

            /**
             * @brief 返回线程ID
             */
            uint32_t getThreadId() const { return m_threadId;}

            /**
             * @brief 返回协程ID
             */
            uint32_t getFiberId() const { return m_fiberId;}

            /**
             * @brief 返回时间
             */
            uint64_t getTime() const { return m_time;}

            /**
             * @brief 返回线程名称
             */
            const std::string& getThreadName() const { return m_threadName;}

            /**
             * @brief 返回日志内容
             */
            std::string getContent() const { return m_ss.str();}

            /**
             * @brief 返回日志器
             */
            std::shared_ptr<Logger> getLogger() const { return m_logger;}

            /**
             * @brief 返回日志级别
             */
            LogLevel::Level getLevel() const { return m_level;}

            /**
             * @brief 返回日志内容字符串流
             */
            std::stringstream& getSS() { return m_ss;}


        private:
            const char* m_file = nullptr;           // 文件名
            uint32_t m_line = 0;                    // 行号
            uint32_t m_threadId = 0;                // 线程号
            uint32_t m_fiberId = 0;                 // 协程号                                                           
            uint32_t m_elapse = 0;                  // 程序启动到现在的时间
            time_t m_time;                          // 时间
            std::string m_threadName;               // 线程名称
            std::stringstream m_ss;                 // 内容
            std::shared_ptr<Logger> m_logger;
                               
            LogLevel::Level m_level;                // 日志等级

    };

    /**
     * @brief 日志事件包装器
     * 使用方法：
     * 通过包装器拿到 event的字符串流，通过 << 写流；
     * 析构时，log流中的信息
     */
    class LogEventWrap {
        public:

            /**
             * @brief 构造函数
             * @param[in] e 日志事件
             */
            LogEventWrap(LogEvent::ptr e);

            /**
             * @brief 析构函数
             */
            ~LogEventWrap();

            /**
             * @brief 获取日志事件
             */
            LogEvent::ptr getEvent() const { return m_event;}

            /**
             * @brief 获取日志内容流
             */
            std::stringstream& getSS();
        private:
            /**
             * @brief 日志事件
             */
            LogEvent::ptr m_event;
};
    
    /*
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名称
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %F 协程id
     *  %N 线程名称
     *
     *  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
     *   
     *  @brief logformatter输出的目的地应该要考虑线程安全
     */
    class LogFormatter{
        public:
            typedef std::shared_ptr<LogFormatter> ptr;

            LogFormatter(const std::string& pattern);

            class FormatItem{
                public:
                    typedef std::shared_ptr<FormatItem> ptr;

                    virtual ~FormatItem() {};

                    virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

            };

            class LogOstream {
                friend class LogFormatter;
                public:
                    typedef std::shared_ptr<LogOstream> ptr;
                    typedef Spinlock MutexType; 

                    LogOstream(const std::ostream& os) : m_os(os){
                    }

                    std::ostream& getOstream() {
                        return const_cast<std::ostream&>(m_os);
                     }

                private:
                    MutexType m_mutex;
                    const std::ostream& m_os;

            };

            std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
            std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

            const std::string getPattern() const {return m_pattern;};

            void init();

            bool isError() const {return m_error;}

            void setOstream(const std::ostream& os) {
                m_logOs = std::make_shared<LogOstream>(os);
            }

            std::ostream& myOstream_format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        private:
            std::string m_pattern;

            std::vector<FormatItem::ptr> m_items;

            LogOstream::ptr m_logOs;

            //
            bool m_error = false;        
    };





    class LogAppender{
        friend class Logger;
        public:
            typedef std::shared_ptr<LogAppender> ptr;
            typedef Spinlock MutexType;

            
            virtual ~LogAppender() {}
            virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
            virtual std::string toYamlString() = 0;


            void setFormatter(LogFormatter::ptr format) {
                MutexType::Lock lock(m_mutex);
                m_logformatter = format;
                if(m_logformatter) {
                    m_hasFormatter = true;
                }
            
            };
            LogFormatter::ptr getFormatter() {
                //可能有人在写
                MutexType::Lock lock(m_mutex);
                return m_logformatter;};

            void setLevel(LogLevel::Level level) {m_level = level;};
            LogLevel::Level getLevel() const {return m_level;}


        protected:
            LogLevel::Level m_level = LogLevel::ERROR;
            LogFormatter::ptr m_logformatter;

            MutexType m_mutex;
            bool m_hasFormatter = false;
    };

    class Logger : public std::enable_shared_from_this<Logger>{
        friend class LogManager;
        public:
            typedef std::shared_ptr<Logger> ptr;

            typedef Spinlock MutexType;
            //Logger() = default;
            Logger(const std::string& name = "root" );

            // ~Logger(){std::cout<<"析构LOGGER"<<std::endl;}
            void log(LogLevel::Level level, LogEvent::ptr event );

            void addAppender(LogAppender::ptr appender);
            void delAppender(LogAppender::ptr appender);

            void clearAppenders() {
                MutexType::Lock lock(m_mutex);
                m_appenders.clear();
            }

            LogLevel::Level getLevel() const {return m_level;};
            void setLevel(LogLevel::Level level) {m_level = level;};

            std::string getName() {return m_name;};

            std::string toYamlString();

            //Formatter 可能有两种形式
            void setFormatter(LogFormatter::ptr val);
            void setFormatter(const std::string& val);

        private:
            std::string m_name;
            
            LogLevel::Level m_level;

            /// Spinlock
            MutexType m_mutex;
            //日志格式器,appender没有formatter，默认使用logger规定的formatter
            LogFormatter::ptr m_formatter;
            std::list<LogAppender::ptr>  m_appenders;
            
            //主日志器
            Logger::ptr m_root;
    };

    class StdoutLogAppender : public LogAppender{
        public:
            typedef std::shared_ptr<StdoutLogAppender> ptr;

            void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
            
            std::string toYamlString() override;
    };

    class FileLogAppender : public LogAppender{

        public:
            typedef std::shared_ptr<FileLogAppender> ptr;
            
            FileLogAppender(const std::string& name) ;
            
            ~FileLogAppender(){
                if(m_filestream)
                    m_filestream.close();
            }
            std::string toYamlString() override;
            void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
            bool reopen();  //检查filestream，先关闭，再打开
        private:
            std::string m_filename;
            std::ofstream m_filestream;

            // 上次重新打开时间
            uint64_t m_lastTime = 0;

    };

    class LogManager{
        public:
            typedef Spinlock MutexType;
            LogManager();

            Logger::ptr getLogger(const std::string& name);

            void init();

            Logger::ptr getRoot() const {return m_root;}

            //将所有的日志器配置转成YAML String
            std::string toYamlString();

            //由loggerManager统一管理logger输出日志的时机
            //并发时，多个logger只能一个一个写
            void log(LogEvent::ptr event, const std::string& name){
                Logger::ptr output_logger = GET_LOG_NAME(name);
                MutexType::Lock lock(m_mutex);
                output_logger->log(event->getLevel(), event);
             }
        private:

            MutexType m_mutex;
            std::map<std::string, Logger::ptr> m_loggers;
            Logger::ptr m_root;

        
    };

}