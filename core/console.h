#pragma once

#include <chrono>
#include <ctime>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <string_view>
#include <utility>

namespace sim::console {
    enum class Level {
        Info, Success, Warning, Error, Debug
    };

    inline std::tm localTime( const std::time_t time ) {
        std::tm local{};
#if defined(_WIN32)
        localtime_s(&local, &time);
#else
        localtime_r(&time, &local);
#endif
        return local;
    }

    inline void timestamp() {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);
        const auto local = localTime(time);
        fmt::print("{:%Y-%m-%d %H:%M:%S} ", local);
    }

    inline void prefix( const std::string_view system, const Level level ) {
        timestamp();
        fmt::print(fmt::fg(fmt::color::white) |
                   fmt::bg(fmt::color::dark_blue),
                   " [{}] ", system
                  );
        switch (level) {
        case Level::Info:
            fmt::print(fmt::fg(fmt::color::white_smoke) |
                       fmt::bg(fmt::color::dodger_blue),
                       " INFO "
                      );
            break;
        case Level::Success:
            fmt::print(fmt::fg(fmt::color::white_smoke) |
                       fmt::bg(fmt::color::forest_green),
                       " OK "
                      );
            break;
        case Level::Warning:
            fmt::print(
                       fmt::fg(fmt::color::black) |
                       fmt::bg(fmt::color::gold),
                       " WARN "
                      );
            break;
        case Level::Error:
            fmt::print(fmt::fg(fmt::color::white_smoke) |
                       fmt::bg(fmt::color::fire_brick),
                       " ERROR "
                      );
            break;
        case Level::Debug:
            fmt::print(fmt::fg(fmt::color::white_smoke) |
                       fmt::bg(fmt::color::slate_gray),
                       " DEBUG "
                      );
            break;
        }
        fmt::print(" ");
    }

    template <typename... Args>
    void log( const std::string_view system, const Level level, fmt::format_string<Args...> message, Args&&... args ) {
        prefix(system, level);
        fmt::print(message, std::forward<Args>(args)...);
        fmt::print("\n");
    }

    template <typename... Args>
    void info( const std::string_view system, fmt::format_string<Args...> message, Args&&... args ) {
        log(system, Level::Info, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void success( const std::string_view system, fmt::format_string<Args...> message, Args&&... args ) {
        log(system, Level::Success, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warning( const std::string_view system, fmt::format_string<Args...> message, Args&&... args ) {
        log(system, Level::Warning, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error( const std::string_view system, fmt::format_string<Args...> message, Args&&... args ) {
        log(system, Level::Error, message, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug( const std::string_view system, fmt::format_string<Args...> message, Args&&... args ) {
        log(system, Level::Debug, message, std::forward<Args>(args)...);
    }
}
