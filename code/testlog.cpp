#include<iostream>
#include<thread>
#include"../lib/log.h"
#include"../lib/utils.h"


int main(int argc, char** argv){

    cocolar::Logger::ptr logger(new cocolar::Logger);
    logger->addAppender(cocolar::LogAppender::ptr(new cocolar::StdoutLogAppender));

    //写入文件流
    cocolar::FileLogAppender::ptr file_appender(new cocolar::FileLogAppender("./log.txt"));
    logger->addAppender(file_appender);
    //cocolar::LogEvent::ptr event(new cocolar::LogEvent(__FILE__, __LINE__,0,cocolar::GetThreadId(),cocolar::GetFiberId(),time(0)));
    //event->getSS()<<"hello coco";
    //logger->log(cocolar::LogLevel::DEBUG,event);
    COCOLAR_LOG_INFO(logger)<<"test macro";

    COCOLAR_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");
    return 0;
}