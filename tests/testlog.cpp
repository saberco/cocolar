#include<iostream>
#include<thread>
#include"../lib/log.h"
#include"../lib/utils.h"


int main(int argc, char** argv){

    cocolar::Logger::ptr logger(new cocolar::Logger);
    logger->addAppender(cocolar::LogAppender::ptr(new cocolar::StdoutLogAppender));


    cocolar::LogFormatter::ptr fmt(new cocolar::LogFormatter("%d%T%p%T%m%n"));
    cocolar::FileLogAppender::ptr file_appender(new cocolar::FileLogAppender("./log.txt"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(cocolar::LogLevel::ERROR);
    //写入文件流
    
    logger->addAppender(file_appender);
    //cocolar::LogEvent::ptr event(new cocolar::LogEvent(__FILE__, __LINE__,0,cocolar::GetThreadId(),cocolar::GetFiberId(),time(0)));
    //event->getSS()<<"hello coco";
    //logger->log(cocolar::LogLevel::DEBUG,event);
    COCOLAR_LOG_INFO(logger)<<"test macro";

    COCOLAR_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = cocolar::LoggerMgr::GetInstance()->getLogger("xx");
    COCOLAR_LOG_INFO(l) << "xxx";
    return 0;
}