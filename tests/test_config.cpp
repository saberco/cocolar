#include"../lib/log.h"
#include"../lib/Config.h"
#include"../lib/utils.h"

cocolar::ConfigVar<int>::ptr g_int_value_config = 
    cocolar::Config::Lookup("systemPort", (int)8888, "system port");


int main(int arge, char ** argv){
    COCOLAR_LOG_INFO(COCOLAR_LOG_ROOT()) << g_int_value_config->getValue();
    COCOLAR_LOG_INFO(COCOLAR_LOG_ROOT()) << g_int_value_config->toString();

    return 0;
}