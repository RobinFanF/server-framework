#include <string.h>
#include <assert.h>

#include "log.h"
#include "utils.h"
//断言封装
#define ASSERT(x) \
    if(!(x)) { \
        LOG_ERROR_STREAM(GET_LOG_ROOT) << "asserttion: " #x \
        << "\nbacktrace:\n"  \
        << frb::BacktraceToString(100, 2, "  "); \
        assert(x); \
    }

#define ASSERT2(x, w) \
    if(!(x)) { \
        LOG_ERROR_STREAM(GET_LOG_ROOT) << "ASSERTION: " #x \
            << "\n" << w \
            << "\nbacktrace:\n" \
            << frb::BacktraceToString(100, 2, "    "); \
        assert(x); \
    }