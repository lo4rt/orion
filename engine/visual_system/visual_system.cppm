module;

#include <algorithm>
#include <expected>
#include <string>
#include <vector>
#include <fstream>

#include <SDL3/SDL.h>
#include "SDL3/SDL_vulkan.h"

#include <volk.h>

#include <core_systems/essential.h>

export module orion.engine.visual_system;

namespace orng
{
    export class visual_system_t
    {
    public:
        std::expected<void, std::string> initialize(SDL_Window* window)
        {
            if (volkInitialize() != VK_SUCCESS)
                return std::unexpected("volkInitialize failed");

            VkApplicationInfo app_info = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = "Orion Engine",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "Orion Engine",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_4
            };

            uint32_t count;
            auto extensions_cstring = SDL_Vulkan_GetInstanceExtensions(&count);
            std::vector<const char*> extensions(extensions_cstring, extensions_cstring + count);
#ifndef NDEBUG
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

            ORLOG_DEBUG("Number of extensions: {}", extensions.size());
            for (const char* extension : extensions)
                ORLOG_DEBUG("\t{}", extension);

#ifndef NDEBUG
            std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
#else
            std::vector<const char*> layers = {};
#endif

            ORLOG_DEBUG("Number of layers: {}", layers.size());
            for (const char* layer : layers)
                ORLOG_DEBUG("\t{}", layer);

#ifndef NDEBUG
            VkDebugUtilsMessengerCreateInfoEXT debug_info = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = 0,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debug_callback,
                .pUserData = nullptr
            };
#else
            VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
#endif

            VkInstanceCreateInfo instance_info = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pNext = &debug_info,
                .flags = 0,
                .pApplicationInfo = &app_info,
                .enabledLayerCount = static_cast<uint32_t>(layers.size()),
                .ppEnabledLayerNames = layers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                .ppEnabledExtensionNames = extensions.data()
            };

            if (vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS)
            {
                return std::unexpected("Failed to create Vulkan instance");
            }
            volkLoadInstance(instance);

#ifndef NDEBUG
            if (vkCreateDebugUtilsMessengerEXT(instance, &debug_info, nullptr, &debug_messanger) != VK_SUCCESS)
                return std::unexpected("Failed to create debug messanger");
#endif


            if (SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface) == false)
                return std::unexpected("Failed to create Vulkan surface");

            uint32_t physical_device_count = 0;
            vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
            if (physical_device_count == 0)
                return std::unexpected("No physical devices found");
            std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
            vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

            ORLOG_DEBUG("Number of physical devices: {}", physical_device_count);


            for (uint32_t i = 0; i < physical_device_count; ++i)
            {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(physical_devices[i], &properties);
                VkPhysicalDeviceFeatures features;
                vkGetPhysicalDeviceFeatures(physical_devices[i], &features);
                ORLOG_DEBUG("\t{}. {}", i+1, properties.deviceName);

                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    physical_device = physical_devices[i];
                    break;
                }
            }

            if (physical_device == VK_NULL_HANDLE) {
                physical_device = physical_devices[0];
            }

            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());
            ORLOG_DEBUG("Number of queue families: {}", queue_family_count);

            uint32_t graphics_family_index = -1;
            for (uint32_t i = 0; i < queue_family_count; i++)
            {
                if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    graphics_family_index = i;
                    break;
                }
            }
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, graphics_family_index, surface, &present_support);
            if (graphics_family_index == -1 || !present_support)
                return std::unexpected("No graphics queue family found");
            ORLOG_DEBUG("Graphics queue family index: {}", graphics_family_index);

            float queue_priority = 1.0f;
            VkDeviceQueueCreateInfo queue_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = graphics_family_index,
                .queueCount = 1,
                .pQueuePriorities = &queue_priority
            };

            const std::vector<const char*> device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            VkPhysicalDeviceFeatures device_features = {};
            VkDeviceCreateInfo device_info = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueCreateInfoCount = 1,
                .pQueueCreateInfos = &queue_info,
                .enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
                .ppEnabledExtensionNames = device_extensions.data(),
                .pEnabledFeatures = &device_features
            };

            if (vkCreateDevice(physical_device, &device_info, nullptr, &device) != VK_SUCCESS)
                return std::unexpected("Failed to create device");

            vkGetDeviceQueue(device, graphics_family_index, 0, &graphics_queue);

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());
            VkSurfaceFormatKHR chosen_format = formats[0];
            for (const auto& format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_SRGB  && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    chosen_format = format;
                    break;
                }
            }
            ORLOG_DEBUG("Image format: {}", static_cast<uint32_t>(chosen_format.format));

            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
            std::vector<VkPresentModeKHR> present_modes(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());

            VkSwapchainCreateInfoKHR swapchain_info = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .surface = surface,
                .minImageCount = std::min(surface_capabilities.minImageCount + 1, surface_capabilities.maxImageCount),
                .imageFormat = chosen_format.format,
                .imageColorSpace = chosen_format.colorSpace,
                .imageExtent = surface_capabilities.currentExtent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .preTransform = surface_capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = VK_PRESENT_MODE_FIFO_KHR,
                .clipped = VK_TRUE,
                .oldSwapchain = VK_NULL_HANDLE
            };

            if (vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain) != VK_SUCCESS)
                return std::unexpected("Failed to create swapchain");

            uint32_t swapchain_image_count;
            vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
            swapchain_images.resize(swapchain_image_count);
            vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images.data());

            swapchain_image_views.resize(swapchain_images.size());

            for (size_t i = 0; i < swapchain_images.size(); ++i)
            {
                VkImageViewCreateInfo view_info = {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = swapchain_images[i],
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = chosen_format.format,
                    .components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },
                    .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
                };
                if (vkCreateImageView(device, &view_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS)
                    return std::unexpected("Failed to create swapchain image view");
            }

            VkAttachmentDescription color_attachment = {
                .format = chosen_format.format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            };

            VkAttachmentReference color_attachment_ref = {
                .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

            VkSubpassDescription subpass = {
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .colorAttachmentCount = 1,
                .pColorAttachments = &color_attachment_ref,
            };

            VkRenderPassCreateInfo render_pass_info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = 1,
                .pAttachments = &color_attachment,
                .subpassCount = 1,
                .pSubpasses = &subpass,
            };




            if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
                return std::unexpected("Failed to create render pass");

            framebuffers.resize(swapchain_image_views.size());

            for (size_t i = 0; i < swapchain_image_views.size(); ++i)
            {
                VkFramebufferCreateInfo framebuffer_info = {
                    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                    .renderPass = render_pass,
                    .attachmentCount = 1,
                    .pAttachments = &swapchain_image_views[i],
                    .width = surface_capabilities.currentExtent.width,
                    .height = surface_capabilities.currentExtent.height,
                    .layers = 1
                };
                if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
                    return std::unexpected("Failed to create framebuffer");
            }

            auto vertex_shader_code = read_shader_file("shaders/triangle.vert.spv");
            auto fragment_shader_code = read_shader_file("shaders/triangle.frag.spv");

            VkShaderModule vertex_shader = create_shader_module(vertex_shader_code);
            VkShaderModule fragment_shader = create_shader_module(fragment_shader_code);

            VkPipelineShaderStageCreateInfo vertex_stage = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertex_shader,
                .pName = "main",
            };

            VkPipelineShaderStageCreateInfo fragment_stage = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragment_shader,
                .pName = "main"
            };

            VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_stage, fragment_stage };

            VkPipelineVertexInputStateCreateInfo vertex_input = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            };

            VkPipelineInputAssemblyStateCreateInfo input_assembly = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            };

            VkViewport viewport = {
                0.0f, 0.0f ,
                static_cast<float>(surface_capabilities.currentExtent.width), static_cast<float>(surface_capabilities.currentExtent.height),
                0.0f, 1.0f
            };
            VkRect2D scissor = { { 0, 0 }, surface_capabilities.currentExtent };
            VkPipelineViewportStateCreateInfo viewport_state = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .pViewports = &viewport,
                .scissorCount = 1,
                .pScissors = &scissor
            };

            VkPipelineRasterizationStateCreateInfo rasterizer = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_NONE,
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .lineWidth = 1.0f,
            };

            VkPipelineMultisampleStateCreateInfo multisampling = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            };

            VkPipelineColorBlendAttachmentState blend_attachment = {
                .blendEnable = VK_FALSE,
                .colorWriteMask =   VK_COLOR_COMPONENT_R_BIT |
                                    VK_COLOR_COMPONENT_G_BIT |
                                    VK_COLOR_COMPONENT_B_BIT |
                                    VK_COLOR_COMPONENT_A_BIT
            };

            VkPipelineColorBlendStateCreateInfo color_blending = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .attachmentCount = 1,
                .pAttachments = &blend_attachment,
            };

            VkPipelineLayoutCreateInfo pipeline_layout_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            };
            if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
                return std::unexpected("Failed to create pipeline layout");

            VkGraphicsPipelineCreateInfo pipeline_info = {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount = 2,
                .pStages = shader_stages,
                .pVertexInputState = &vertex_input,
                .pInputAssemblyState = &input_assembly,
                .pViewportState = &viewport_state,
                .pRasterizationState = &rasterizer,
                .pMultisampleState = &multisampling,
                .pColorBlendState = &color_blending,
                .layout = pipeline_layout,
                .renderPass = render_pass,
                .subpass = 0,
            };

            if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) != VK_SUCCESS)
                return std::unexpected("Failed to create graphics pipeline");

            vkDestroyShaderModule(device, vertex_shader, nullptr);
            vkDestroyShaderModule(device, fragment_shader, nullptr);

            VkCommandPoolCreateInfo pool_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = graphics_family_index
            };
            if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS)
                return std::unexpected("Failed to create command pool");

            command_buffers.resize(swapchain_image_views.size());
            VkCommandBufferAllocateInfo alloc_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = command_pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = static_cast<uint32_t>(command_buffers.size())
            };
            if (vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()) != VK_SUCCESS)
                return std::unexpected("Failed to allocate command buffers");

            VkSemaphoreCreateInfo semaphore_info = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
            };
            VkFenceCreateInfo fence_info = {
                .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                .flags = VK_FENCE_CREATE_SIGNALED_BIT
            };

            if (vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphore) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphore) != VK_SUCCESS ||
                vkCreateFence(device, &fence_info, nullptr, &in_flight_fence) != VK_SUCCESS)
            {
                return std::unexpected("Failed to create synchronization objects");
            }

            return {};
        }

        void draw_frame()
        {
            vkWaitForFences(device, 1, &in_flight_fence, VK_TRUE, UINT64_MAX);
            vkResetFences(device, 1, &in_flight_fence);

            uint32_t image_index;
            vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE, &image_index);

            vkResetCommandBuffer(command_buffers[image_index], 0);
            record_command_buffer(command_buffers[image_index], image_index);

            VkSemaphore wait_semaphores[] = { image_available_semaphore };
            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            VkSemaphore signal_semaphores[] = { render_finished_semaphore };
            VkSubmitInfo submit_info = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = wait_semaphores,
                .pWaitDstStageMask = wait_stages,
                .commandBufferCount = 1,
                .pCommandBuffers = &command_buffers[image_index],
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = signal_semaphores
            };

            if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fence) != VK_SUCCESS)
                ORLOG_ERROR("Failed to submit draw command buffer");

            VkSwapchainKHR swapchains[] = { swapchain };
            VkPresentInfoKHR present_info = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = signal_semaphores,
                .swapchainCount = 1,
                .pSwapchains = swapchains,
                .pImageIndices = &image_index,
            };

            vkQueuePresentKHR(graphics_queue, &present_info);
        }

        void record_command_buffer(VkCommandBuffer command_buffer, uint32_t image_index)
        {
            VkCommandBufferBeginInfo begin_info = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            };
            vkBeginCommandBuffer(command_buffer, &begin_info);

            VkClearValue clear_color = {{0.1f, 0.1f, 0.1f, 1.0f}};
            VkRenderPassBeginInfo render_pass_info = {
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = render_pass,
                .framebuffer = framebuffers[image_index],
                .renderArea = { { 0, 0 }, surface_capabilities.currentExtent },
                .clearValueCount = 1,
                .pClearValues = &clear_color
            };

            vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
            vkCmdDraw(command_buffer, 3, 1, 0, 0);
            vkCmdEndRenderPass(command_buffer);

            if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
                ORLOG_ERROR("Failed to record command buffer");
        }

        std::vector<char> read_shader_file(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);
            if (!file.is_open()) {
                ORLOG_ERROR("Failed to open file: {}", filename);
                return {};
            }
            auto size = static_cast<size_t>(file.tellg());
            std::vector<char> buffer(size);
            file.seekg(0);
            file.read(buffer.data(), size);
            file.close();
            return buffer;
        }

        VkShaderModule create_shader_module(const std::vector<char>& code)
        {
            VkShaderModuleCreateInfo create_info = {
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const uint32_t*>(code.data())
            };
            VkShaderModule shader_module;
            if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
            {
                ORLOG_ERROR("Failed to create shader module");
                return VK_NULL_HANDLE;
            }
            return shader_module;
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
        {
            ORLOG_DEBUG("Validation Layers -> {}", pCallbackData->pMessage);
            return VK_FALSE; // Всегда возвращаем VK_FALSE
        }

        void shutdown()
        {
            for (auto framebuffer : framebuffers)
                vkDestroyFramebuffer(device, framebuffer, nullptr);
            vkDestroyRenderPass(device, render_pass, nullptr);
            for (auto view : swapchain_image_views)
                vkDestroyImageView(device, view, nullptr);
            vkDestroySwapchainKHR(device, swapchain, nullptr);
            vkDestroyDevice(device, nullptr);
            vkDestroySurfaceKHR(instance, surface, nullptr);
#ifndef NDEBUG
            vkDestroyDebugUtilsMessengerEXT(instance, debug_messanger, nullptr);
#endif
            vkDestroyInstance(instance, nullptr);
        }


    private:
        VkInstance instance = VK_NULL_HANDLE;

#ifndef NDEBUG
        VkDebugUtilsMessengerEXT debug_messanger = VK_NULL_HANDLE;
#endif

        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphics_queue = VK_NULL_HANDLE;

        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> swapchain_images = {};
        std::vector<VkImageView> swapchain_image_views = {};

        VkRenderPass render_pass = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> framebuffers = {};

        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
        VkPipeline graphics_pipeline = VK_NULL_HANDLE;

        VkCommandPool command_pool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> command_buffers = {};

        VkSemaphore image_available_semaphore = VK_NULL_HANDLE;
        VkSemaphore render_finished_semaphore = VK_NULL_HANDLE;
        VkFence in_flight_fence = VK_NULL_HANDLE;
    };
}