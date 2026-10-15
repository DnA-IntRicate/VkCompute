#pragma once
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <ctime>


namespace vkc
{
    enum class LogLevel
    {
        Trace,
        Info,
        Warn,
        Error
    };

    constexpr const char* ToString(LogLevel level) noexcept
    {
        switch (level)
        {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
        }

        return "UNKNOWN";
    }

    inline std::string Timestamp() noexcept
    {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
        localtime_s(&tm, &t);

        return fmt::format("{:%H:%M:%S}", tm);
    }

    template<typename... Args>
    inline void Log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) noexcept
    {
        FILE* out = (level == LogLevel::Error) ? stderr : stdout;
        fmt::print(out, "[{}] [{}]: {}\n", Timestamp(), ToString(level), fmt::format(fmt, std::forward<Args>(args)...));
    }
}  // namespace vkc
