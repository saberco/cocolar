#ifndef __COCOLAR_LOG_H__
#define __COCOLAR_LOG_H__

#include<iostream>
#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<sstream>
#include<fstream>
#include<vector>
#include <stdarg.h>
#include <stdio.h>
#include<map>
#include"../lib/singleton.h"
#include"../lib/utils.h"

#define COCOLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel()<=level) \
        cocolar::LogEventWrap(cocolar::LogEvent::ptr(new cocolar::LogEvent(logger, level \
            , __FILE__, __LINE__, 0, cocolar::GetThreadId() \
            ,cocolar::GetFiberId(), time(0)))).getSS()

#define COCOLAR_LOG_DEBUG(logger) COCOLAR_LOG_LEVEL(logger, cocolar::LogLevel::DEBUG)
#define COCOLAR_LOG_INFO(logger) COCOLAR_LOG_LEVEL(logger, cocolar::LogLevel::INFO)
#define COCOLAR_LOG_WARN(logger) COCOLAR_LOG_LEVEL(logger, cocolar::LogLevel::WARN)
#define COCOLAR_LOG_ERROR(logger) COCOLAR_LOG_LEVEL(logger, cocolar::LogLevel::ERROR)
#define COCOLAR_LOG_FATAL(logger) COCOLAR_LOG_LEVEL(logger, cocolar::LogLevel::FATAL)


#define COCOLAR_LOG_FMT_LEVEL(logger, level, fmt,...) \
    if(logger->getLevel()<=level) \
        cocolar::LogEventWrap(cocolar::LogEvent::ptr(new cocolar::LogEvent(logger, level \
            , __FILE__, __LINE__, 0, cocolar::GetThreadId() \
            ,cocolar::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define COCOLAR_LOG_FMT_DEBUG(logger, fmt, ...) COCOLAR_LOG_FMT_LEVEL(logger, cocolar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define COCOLAR_LOG_FMT_INFO(logger, fmt, ...) COCOLAR_LOG_FMT_LEVEL(logger, cocolar::LogLevel::INFO, fmt, __VA_ARGS__)
#define COCOLAR_LOG_FMT_WARN(logger, fmt, ...) COCOLAR_LOG_FMT_LEVEL(logger, cocolar::LogLevel::WARN, fmt, __VA_ARGS__)
#define COCOLAR_LOG_FMT_ERROR(logger, fmt, ...) COCOLAR_LOG_FMT_LEVEL(logger, cocolar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define COCOLAR_LOG_FMT_FATAL(logger, fmt, ...) COCOLAR_LOG_FMT_LEVEL(logger, cocolar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define COCOLAR_LOG_ROOT() cocolar::LoggerMgr::GetInstance()->getRoot()

namespace cocolar{
class Logger;


class LogLevel{
public:
    //????????????
    enum Level{
        UNKNOW=0,
        DEBUG=1,
        INFO=2,
        WARN=3,
        ERROR=4,
        FATAL=5
    };

    static const char* ToString(LogLevel::Level level);
};

//??????????????????????????????
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,const char* file, int32_t m_line, uint32_t elapse
        ,uint32_t thread_id,uint32_t fiber_id, uint32_t time);
    const char * getFile() const {return m_file;}
    int32_t getLine() const {return m_line;}
    uint32_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const {return m_threadId;}
    uint32_t getFiberId() const {return m_fiberId;}
    uint64_t getTime() const {return m_time;}

    std::string getContent() const {return m_ss.str();}
    std::shared_ptr<Logger> getLogger(){return m_logger;}
    LogLevel::Level getLevel(){return m_level;}
    std::stringstream& getSS() {return m_ss;}

    void format(const char * fmt,...);
    void format(const char * fmt, va_list al);
private:
    const char * m_file = nullptr;  //?????????
    int32_t m_line = 0;             //??????
    uint32_t m_elapse = 0;          //???????????????????????????????????????
    uint32_t m_threadId = 0;        //?????????
    uint32_t m_fiberId = 0;         //?????????
    uint64_t m_time;                //?????????
    std::stringstream m_ss;          //??????

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};


class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();

    LogEvent::ptr getEvent() const {return m_event;}
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

//???????????????
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr; 
    LogFormatter(const std::string& pattern);
    // %t   %thread_id  %m%n
    std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level ,LogEvent::ptr event);
public:
    //????????????????????????????????????
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr; 
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)=0;
    };
    void init();
private:
    std::string m_pattern;
    //???????????????
    std::vector<FormatItem::ptr> m_items;
};



//?????????????????????
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}

    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event)=0;
    void setFormatter(LogFormatter::ptr val){m_formatter = val;}
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}
    LogFormatter::ptr getFormatter() const {return m_formatter;}
protected:
    LogLevel::Level m_level;        //????????????
    LogFormatter::ptr m_formatter;
};


//???????????????
class Logger : public std::enable_shared_from_this<Logger>{
public:
    //????????????
    typedef std::shared_ptr<Logger> ptr;  

    Logger(const std::string name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);
    //???????????????appender
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}

    const std::string& getName() const {return m_name;} 
private:
    //?????????
    std::string m_name;         //????????????
    std::list<LogAppender::ptr> m_appenders;           //???????????????????????????
    LogLevel::Level m_level;    //?????????????????????????????????
    LogFormatter::ptr m_formatter;
};






//??????????????????
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr; 
    //override??????????????????????????????????????????????????????????????????????????????????????????????????????
    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
};

//???????????????
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr; 
    FileLogAppender(const std::string& filename);
    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
    //???????????????????????????????????????true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};


class LoggerManage{
public:
    LoggerManage();
    Logger::ptr getLogger(const std::string & name);

    void init();
    Logger::ptr getRoot() const {return m_root;}
    
private:

    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;


};

typedef cocolar::Singleton<LoggerManage> LoggerMgr;



}









#endif