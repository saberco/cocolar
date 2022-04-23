#ifndef __COCO_UTILS_H__
#define __COCO_UTILS_H__
#include<sys/syscall.h>
#include<pthread.h>
#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<stdint.h>

namespace cocolar{

pid_t GetThreadId();
uint32_t GetFiberId();

}








#endif