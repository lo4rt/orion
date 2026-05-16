import orion.engine.logging;

#if defined(ORION_ENABLE_TRACE_LOGS)
    #define ORLOG_TRACE(...) ::orng::logging::trace(__VA_ARGS__)
#else
    #define ORLOG_TRACE(...)
#endif

#if !defined(NDEBUG)
    #define ORLOG_DEBUG(...) ::orng::logging::debug(__VA_ARGS__)
#else
    #define ORLOG_DEBUG(...)
#endif

#define ORLOG_INFO(...) ::orng::logging::info(__VA_ARGS__)
#define ORLOG_WARN(...) ::orng::logging::warn(__VA_ARGS__)
#define ORLOG_ERROR(...) ::orng::logging::error(__VA_ARGS__)
#define ORLOG_CRITICAL(...) ::orng::logging::critical(__VA_ARGS__)
