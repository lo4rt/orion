module;

#include <spdlog/spdlog.h>

export module orion.logging;

export namespace orion::logging
{

    template<typename... Args>
    void trace(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        spdlog::trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        spdlog::error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(spdlog::format_string_t<Args...> fmt, Args&&... args)
    {
        spdlog::critical(fmt, std::forward<Args>(args)...);
    }

}