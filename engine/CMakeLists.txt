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

find_program(GLSLC_EXECUTABLE glslc REQUIRED)

function(compile_shader SHADER_SOURCE)
    get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME)
    set(SHADER_OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${SHADER_NAME}.spv")

    add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            COMMAND ${GLSLC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/visual_system/shaders/${SHADER_SOURCE} -o ${SHADER_OUTPUT}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/visual_system/shaders/${SHADER_SOURCE}
            COMMENT "Compiling shader: ${SHADER_SOURCE} -> ${SHADER_OUTPUT}"
    )

    target_sources(orion-engine PRIVATE ${SHADER_OUTPUT})
endfunction()

compile_shader(triangle.vert)
compile_shader(triangle.frag)