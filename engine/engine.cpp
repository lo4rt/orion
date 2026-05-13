module;

#include <iostream>

module engine;

void Engine::initialize()
{
    std::cout << "Initializing engine..." << std::endl;
    std::cout << "Engine initialized successfully." << std::endl;
}

void Engine::run()
{
    std::cout << "Running engine..." << std::endl;
}

void Engine::shutdown()
{
    std::cout << "Shutting down engine..." << std::endl;
}