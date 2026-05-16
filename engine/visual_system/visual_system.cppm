module;

#include <SDL3/SDL.h>

#include <expected>
#include <memory>
#include <string>
#include <vector>

export module orion.engine.visual_system;

namespace orng
{
    export class visual_system_t
    {
    public:
        void initialize()
        {

        }

        std::expected<void, std::string> create_vulkan_window(std::string_view title, int width, int height)
        {
            window_ptr_t window(
                SDL_CreateWindow(
                    title.data(),
                    width,
                    height,
                    SDL_WINDOW_VULKAN
                ),
                &SDL_DestroyWindow
            );

            if (!window)
            {
                return std::unexpected(std::string("Failed to create window: ") + SDL_GetError());
            }

            windows_.push_back(std::move(window));

            return {};
        }
    private:
        using window_ptr_t = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
        std::vector<window_ptr_t> windows_;
    };
}