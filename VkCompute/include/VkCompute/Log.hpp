#pragma once
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <utility>


namespace vkc
{
    enum class LogLevel
    {
        Trace,
        Info,
        Warn,
        Error,
        Critical,
        Success
    };

    constexpr const char* ToString(LogLevel level) noexcept
    {
        switch (level)
        {
            case LogLevel::Trace:    return "TRACE";
            case LogLevel::Info:     return "INFO";
            case LogLevel::Warn:     return "WARN";
            case LogLevel::Error:    return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            case LogLevel::Success:  return "SUCCESS";
        }

        return "UNKNOWN";
    }

    inline std::tm Timestamp() noexcept
    {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
        localtime_s(&tm, &t);

        return tm;
    }

    template<LogLevel Level, fmt::formattable... Args>
    void Log(fmt::format_string<Args...> fmt, Args&&... args)
    {
        FILE* out = stdout;
        if constexpr (Level == LogLevel::Error)
            out = stderr;

        fmt::text_style ts;
        if constexpr (Level == LogLevel::Info)
            ts |= fmt::fg(fmt::color::cyan);
        else if constexpr (Level == LogLevel::Warn)
            ts |= fmt::fg(fmt::color::yellow);
        else if constexpr (Level == LogLevel::Error)
            ts |= fmt::fg(fmt::color::red);
        else if constexpr (Level == LogLevel::Critical)
            ts |= fmt::fg(fmt::color::white) | fmt::bg(fmt::color::red);
        else if constexpr (Level == LogLevel::Success)
            ts |= fmt::fg(fmt::color::lime_green);

        constexpr const char* levelStr = ToString(Level);

        fmt::memory_buffer buf;
        fmt::format_to(std::back_inserter(buf), ts, "[{:%H:%M:%S}] [{}]: ", Timestamp(), levelStr);
        fmt::format_to(std::back_inserter(buf), ts, fmt, std::forward<Args>(args)...);
        buf.push_back('\n');
        fwrite(buf.data(), 1, buf.size(), out);

        // We can actually just call this but...
        // TODO(Adam): Document this correctly!
        // fmt::print(out, ts, "[{:%H:%M:%S}] [{}]: {}\n", Timestamp(), levelStr, fmt::format(fmt, std::forward<Args>(args)...));
    }

    template<fmt::formattable... Args>
    void Trace(fmt::format_string<Args...> fmt, Args&&... args)
    {
        Log<LogLevel::Trace>(fmt, std::forward<Args>(args)...);
    }

    template<fmt::formattable... Args>
    void Info(fmt::format_string<Args...> fmt, Args&&... args)
    {
        Log<LogLevel::Info>(fmt, std::forward<Args>(args)...);
    }

    template<fmt::formattable... Args>
    void Warn(fmt::format_string<Args...> fmt, Args&&... args)
    {
        Log<LogLevel::Warn>(fmt, std::forward<Args>(args)...);
    }

    template<fmt::formattable... Args>
    void Error(fmt::format_string<Args...> fmt, Args&&... args)
    {
        Log<LogLevel::Error>(fmt, std::forward<Args>(args)...);
    }

    template<fmt::formattable... Args>
    void Critical(fmt::format_string<Args...> fmt, Args&&... args)
    {
        Log<LogLevel::Critical>(fmt, std::forward<Args>(args)...);
    }

    template<fmt::formattable... Args>
    void Success(fmt::format_string<Args...> fmt, Args&&... args)
    {
        Log<LogLevel::Success>(fmt, std::forward<Args>(args)...);
    }
}  // namespace vkc
