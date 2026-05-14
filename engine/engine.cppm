export module orion.engine;

#include <core_systems/logging/logging.h>


namespace orion
{
    export class engine
    {
    public:
        void initialize()
        {



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
