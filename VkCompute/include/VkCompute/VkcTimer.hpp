#pragma once
#include <chrono>
#include <cstdint>


namespace vkc
{
    /**
     * @brief A high-resolution RAII scoped timer.
     */
    template <typename Duration = std::chrono::microseconds>
    class VkcTimer
    {
    private:
        using Clock     = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

    public:
        /**
         * @brief Constructs and starts the timer.
         */
        explicit VkcTimer() noexcept
            : m_Start(Clock::now())
        {
        }
        
        ~VkcTimer() = default;

        // Forbid copy-assignment
        constexpr VkcTimer(const VkcTimer&)            = delete;
        constexpr VkcTimer& operator=(const VkcTimer&) = delete;

        // Forbid move-assignment
        constexpr VkcTimer(VkcTimer&&)            = delete;
        constexpr VkcTimer& operator=(VkcTimer&&) = delete;

        /**
         * @brief Gets the elapsed duration without stopping the timer.
         */
        constexpr uint64_t Elapsed() const noexcept
        {
            const Duration elapsed = std::chrono::duration_cast<Duration>(Clock::now() - m_Start);
            return static_cast<uint64_t>(elapsed.count());
        }

    private:
        TimePoint m_Start;
    };
}  // namespace vkc
