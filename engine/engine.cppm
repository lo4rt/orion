module;

#include <iostream>

export module orion.engine;

#include <core_systems/logging/logging.h>

namespace Orion
{
    export class Engine
    {
    public:
        void initialize()
        {
            LOG_INFO("Initializing engine...");
            LOG_INFO("Engine initialized successfully.");
        }
        void run()
        {
            LOG_INFO("Running engine...");
        }
        void shutdown()
        {
            LOG_INFO("Shutting down engine...");
        }

    private:

    };
}