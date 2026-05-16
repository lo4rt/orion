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
        void process_event(const SDL_Event& event)
        {
            switch (event.type)
            {
                default:
                    break;
            }
        }

        void update_all_systems(float delta_time)
        {

        }

        void render()
        {

        }


    private:
        std::expected<void, std::string> initialize_core_systems()
        {
            if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
            {
                return std::unexpected(std::string("Failed to initialize SDL: ") + SDL_GetError());
            }
            return {};
        }

    public:
        std::expected<void, std::string> initialize()
        {
            auto core_systems_result = initialize_core_systems();
            if (!core_systems_result) { return std::unexpected(core_systems_result.error()); }


            ORLOG_INFO("Engine initialized successfully. All systems are ready.");
            return {};
        }
        
        void shutdown()
        {
            ORLOG_INFO("Shutting down engine...");
            SDL_Quit();
        }

    private:
        visual_system_t visual_system;
    public:
        visual_system_t& get_visual_system()
        {
            return visual_system;
        }
    };
}
