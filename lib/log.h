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



namespace cocolar{
class Logger;


class LogLevel{
public:
    //日志级别
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

//日志事件存放日志属性
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
    const char * m_file = nullptr;  //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序开始启动到现在的毫秒数
    uint32_t m_threadId = 0;        //线程号
    uint32_t m_fiberId = 0;         //协程号
    uint64_t m_time;                //时间戳
    std::stringstream m_ss;          //内容

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

//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr; 
    LogFormatter(const std::string& pattern);
    // %t   %thread_id  %m%n
    std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level ,LogEvent::ptr event);
public:
    //虚基类
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr; 
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event)=0;
    };
    void init();
private:
    std::string m_pattern;
    //具体的子类
    std::vector<FormatItem::ptr> m_items;
};



//日志输入目的地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}

    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event)=0;
    void setFormatter(LogFormatter::ptr val){m_formatter = val;}
    LogFormatter::ptr getFormatter() const {return m_formatter;}
protected:
    LogLevel::Level m_level;        //日志级别
    LogFormatter::ptr m_formatter;
};


//日志输出器
class Logger : public std::enable_shared_from_this<Logger>{
public:
    //智能指针
    typedef std::shared_ptr<Logger> ptr;  

    Logger(const std::string name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);
    //添加和删除appender
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}

    const std::string& getName() const {return m_name;} 
private:
    //文件名
    std::string m_name;         //日志名称
    std::list<LogAppender::ptr> m_appenders;           //输出到目的地的集合
    LogLevel::Level m_level;    //满足日志级别的才会输出
    LogFormatter::ptr m_formatter;
};






//输出到控制台
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr; 
    //override表示是重写的父类的虚函数，如果不是重写，会无法通过编译，防止多态错误
    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
};

//输出到文件
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr; 
    FileLogAppender(const std::string& filename);
    virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;
    //重新打开文件，打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};



}









#endif