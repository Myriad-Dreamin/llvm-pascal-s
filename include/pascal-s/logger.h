
#ifndef MY_LOGGER_IMPL
#define MY_LOGGER_IMPL

#include <dep/stl.h>
#include <fmt/core.h>

struct Logger {

    std::ostream &os = std::cout;
#define level_log(func_name, x)     void func_name(const char*file, const char*func, const int line,\
    string_view s) {\
        os << fmt::format("{0}:{1}:{2} {3} msg:", file, func, line,  #func_name) << s;\
        if (x) exit(1);\
    }


#define level_logf(func_name)     template <typename S=char, typename... Args>\
    void func_name(const char *file, const char *func, const int line, const char *fmt, Args &&...args) {\
        warning(file, func, line, fmt::format(fmt, args...));\
    }

    level_log(fatal, true)
    level_log(critical, false)
    level_log(error, false)
    level_log(debug, false)
    level_log(warning, false)
    level_log(info, false)

    level_logf(fatalf)
    level_logf(criticalf)
    level_logf(errorf)
    level_logf(debugf)
    level_logf(warningf)
    level_logf(infof)
};

#define lfatalf(logger, ...) logger.fatalf(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define fatalf(...) lfatalf(logger, ##__VA_ARGS__)
#define lcriticalf(logger, ...) logger.criticalf(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define criticalf(...) lcriticalf(logger, ##__VA_ARGS__)
#define lerrorf(logger, ...) logger.errorf(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define errorf(...) lerrorf(logger, ##__VA_ARGS__)
#define ldebugf(logger, ...) logger.debugf(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define debugf(...) ldebugf(logger, ##__VA_ARGS__)
#define lwarningf(logger, ...) logger.warningf(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define warningf(...) lwarningf(logger, ##__VA_ARGS__)
#define linfof(logger, ...) logger.infof(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define infof(...) linfof(logger, ##__VA_ARGS__)


#endif