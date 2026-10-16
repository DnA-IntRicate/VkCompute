#pragma once
#include "Log.hpp"
#include <fmt/format.h>
#include <fmt/color.h>
#include <cstdio>
#include <exception>
#include <iterator>
#include <source_location>
#include <string_view>


namespace vkc
{
    /**
     * @brief Handles assertions.
     * Rather use the macro `VKC_ASSERT` since it gets disabled during release.
     */
    inline void Assert(bool cond, std::string_view msg = "", const std::source_location& loc = std::source_location::current()) noexcept
    {
        if (cond)
            return;

        constexpr fmt::text_style ts = fmt::fg(fmt::color::white) | fmt::bg(fmt::color::red);

        fmt::memory_buffer buf;
        fmt::format_to(
            std::back_inserter(buf),
            ts,
            "[{:%H:%M:%S}] [ASSERTION FAILED] {}:{} in {}",
            Timestamp(),
            loc.file_name(),
            loc.line(),
            loc.function_name()
        );

        if (!msg.empty())
            fmt::format_to(std::back_inserter(buf), ts, ": {}", msg);

        buf.push_back('\n');
        fwrite(buf.data(), 1, buf.size(), stderr);

        std::terminate();
    }
}  // namespace vkc

#if defined(VKC_DEBUG) || defined(VKC_DEV)
#    define VKC_ASSERT(cond, msg) ::vkc::Assert(cond, msg)
#else
#    define VKC_ASSERT(cond, msg)
#endif  // VKC_DEBUG
