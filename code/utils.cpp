#include "../lib/utils.h"



namespace cocolar{

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}


uint32_t GetFiberId(){
    return 0;
}
}