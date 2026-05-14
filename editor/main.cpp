#include <iostream>

import orion.engine;

int main()
{
    Orion::Engine engine;
    engine.initialize();
    engine.run();
    engine.shutdown();
}
