import orion.logging;

#if defined(ORION_ENABLE_TRACE_LOGS)
    #define LOG_TRACE(...) ::orion::logging::trace(__VA_ARGS__)
#else
    #define LOG_TRACE(...)
#endif

#if !defined(NDEBUG)
    #define LOG_DEBUG(...) ::orion::logging::debug(__VA_ARGS__)
#else
    #define LOG_DEBUG(...)
#endif

#define LOG_INFO(...) ::orion::logging::info(__VA_ARGS__)
#define LOG_WARN(...) ::orion::logging::warn(__VA_ARGS__)
#define LOG_ERROR(...) ::orion::logging::error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::orion::logging::critical(__VA_ARGS__)