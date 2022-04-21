#ifndef __COCOLAR_LOG_H__
#define __COCOLAR_LOG_H__

#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<sstream>
#include<fstream>



namespace cocolar{

class LogLevel{
public:
    //日志级别
    enum Level{
        DEBUG=1,
        INFO=2,
        WARN=3,
        ERROR=4,
        FATAL=5
    };
};

//日志事件存放日志属性
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();


private:
    const char * m_file = nullptr;  //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序开始启动到现在的毫秒数
    uint32_t m_threadId = 0;        //线程号
    uint32_t m_fiberId = 0;         //协程号
    uint64_t m_time;                //时间戳
    std::string m_content;          //内容
};

//日志输入目的地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}

    virtual void log(LogLevel::Level level, LogEvent::ptr event);
    void setFormatter(LogFormatter::ptr val){m_formatter = val;}
    LogFormatter::ptr getFormatter() const {return m_formatter;}
protected:
    LogLevel::Level m_level;        //日志级别
    LogFormatter::ptr m_formatter;
};


//日志输出器
class Logger{
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
private:
    //文件名
    std::string m_name;         //日志名称
    std::list<LogAppender::ptr> m_appenders;           //输出到目的地的集合
    LogLevel::Level m_level;    //满足日志级别的才会输出
};



//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr; 
    std::string format(LogEvent::ptr event);
};


//输出到控制台
class StdoutLogAppender::public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr; 
    //override表示是重写的父类的虚函数，如果不是重写，会无法通过编译，防止多态错误
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;
};

//输出到文件
class FileLogAppender::public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr; 
    FileLogAppender(const std::string& filename);
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;
    //重新打开文件，打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};



}









#endif