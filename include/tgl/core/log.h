#ifndef TGL_CORE_LOG_H
#define TGL_CORE_LOG_H

#include <cstdio>
#include <cstdarg>

namespace tgl {

enum class LogLevel { Debug, Info, Warn, Error };

namespace log {

inline LogLevel g_min_level = LogLevel::Info;

inline void set_level(LogLevel level) { g_min_level = level; }

inline void output(LogLevel level, const char* prefix, const char* fmt, va_list args) {
    if (level < g_min_level) return;
    fprintf(stderr, "[TGL %s] ", prefix);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

inline void debug(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    output(LogLevel::Debug, "DEBUG", fmt, args);
    va_end(args);
}

inline void info(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    output(LogLevel::Info, "INFO", fmt, args);
    va_end(args);
}

inline void warn(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    output(LogLevel::Warn, "WARN", fmt, args);
    va_end(args);
}

inline void error(const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    output(LogLevel::Error, "ERROR", fmt, args);
    va_end(args);
}

} // namespace log
} // namespace tgl

#endif // TGL_CORE_LOG_H
