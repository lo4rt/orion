add_library(orion-engine)
target_link_libraries(orion-engine
    PRIVATE
        SDL3::SDL3
        volk::volk
        spdlog::spdlog
)

target_include_directories(orion-engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(orion-engine
    PUBLIC
        $<$<CONFIG:Debug>:ORION_ENABLE_TRACE_LOGS>
)

# engine
target_sources(orion-engine
    PUBLIC FILE_SET CXX_MODULES FILES
        engine.cppm
)

# logging module
target_sources(orion-engine
    PUBLIC FILE_SET CXX_MODULES FILES
        core_systems/logging/logging.cppm
    PRIVATE
        core_systems/logging/logging.h
)

# essential
target_sources(orion-engine
    PRIVATE
        core_systems/essential.h
)

# visual system
target_sources(orion-engine
    PUBLIC FILE_SET CXX_MODULES FILES
        visual_system/visual_system.cppm
)