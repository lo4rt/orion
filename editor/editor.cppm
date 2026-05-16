module;

#include <SDL3/SDL.h>

#include <expected>
#include <string>

#include <core_systems/essential.h>

export module orion.editor;

import orion.engine;


export namespace ored
{
    class editor_t
    {
    private:
        orng::engine_t engine = {};
        SDL_Window* main_window = nullptr;

    public:
        std::expected<void, std::string> initialize()
        {
            if (SDL_Init(SDL_INIT_VIDEO) == false)
            {
                return std::unexpected(SDL_GetError());
            }
            main_window = SDL_CreateWindow(
                "Orion Editor",
                1280, 720,
                SDL_WINDOW_VULKAN
            );

            if (auto init_result = engine.initialize(main_window); !init_result)
            {
                SDL_Quit();
                return std::unexpected(init_result.error());
            }


            if (!main_window) { return std::unexpected(SDL_GetError()); }

            return {};
        }

        void shutdown()
        {
            engine.shutdown();
            SDL_DestroyWindow(main_window);
            SDL_Quit();
        }


    private:
        bool is_running = true;

    public:
        void run()
        {
            Uint64 last_time = SDL_GetPerformanceCounter();
            Uint64 timer_frequency = SDL_GetPerformanceFrequency();
            float delta_time = 0;
            Uint64 current_time = 0;

            while (is_running)
            {
                process_os_events();

                current_time = SDL_GetPerformanceCounter();
                delta_time =    static_cast<float>(static_cast<double>(current_time - last_time) /
                                                    static_cast<double>(timer_frequency));
                last_time = current_time;

                if (delta_time > 0.1f) delta_time = 0.1f;

                engine.update_all_systems(delta_time);
            }
        }

    private:
        void process_os_events()
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_EVENT_QUIT:
                        is_running = false;
                        break;
                    case SDL_EVENT_KEY_DOWN:
                    case SDL_EVENT_KEY_UP:
                        process_input_events(event);
                        break;
                    default:
                        break;
                }
            }
        }
        void process_input_events(SDL_Event& event)
        {
            switch (event.key.key)
            {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;
                default:
                    break;
            }
        }

    };
}