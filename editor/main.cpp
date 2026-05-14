#include <iostream>

import orion.engine;

int main()
{
    orion::engine engine;
    engine.initialize();
    engine.run();
    engine.shutdown();
}
