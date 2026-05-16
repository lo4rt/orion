module;

#include <SDL3/SDL.h>

#include <string>
#include <expected>

export module orion.engine;

import orion.engine.visual_system;
#include <core_systems/essential.h>

namespace orng
{
    export class engine_t
    {
    public:
        visual_system_t visual_system;

    public:
        std::expected<void, std::string> initialize(SDL_Window* window)
        {
            if (auto init_result = initialize_core_systems(); !init_result)
                return std::unexpected(init_result.error());

            if (auto init_result = visual_system.initialize(window); !init_result)
                return std::unexpected(init_result.error());

            ORLOG_INFO("Engine initialized successfully. All systems are ready.");
            return {};
        }
    private:
        std::expected<void, std::string> initialize_core_systems()
        {
            orng::logging::initialize();


            return {};
        }

    public:
        void shutdown()
        {
            ORLOG_INFO("Shutting down engine...");
        }

    public:
        void update_all_systems(float delta_time)
        {
            visual_system.draw_frame();

        }
    };
}
