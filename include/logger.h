
#ifndef MY_LOGGER_IMPL
#define MY_LOGGER_IMPL

struct Logger {
    std::ostream &os = std::cout;

    void fatal(const std::string_view s) {
        os << s;
        exit(1);
    }

    void critical(const std::string_view s) {
        os << s;
    }

    void error(const std::string_view s) {
        os << s;
    }

    void debug(const std::string_view s) {
        os << s;
    }

    void warning(const std::string_view s) {
        os << s;
    }

    void info(const std::string_view s) {
        os << s;
    }

    // todo: will delete later
    void warningf(const char *__restrict __format, ...) {
        char *buf = new char[1024];
        va_list args;
        va_start(args, __format);
        vsprintf(buf, __format, args);
        va_end(args);
        warning(buf);
        delete[] buf;
    }
};

#endif