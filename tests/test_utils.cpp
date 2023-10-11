
#include "../include/log.h"
#include "../include/macro.h"

frb::Logger::ptr g_logger = GET_LOG_ROOT;

void test_assert(){
    ASSERT(0)
}

int main(){
    test_assert();
}