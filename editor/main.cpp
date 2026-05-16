#include <string>
#include <expected>

#include <core_systems/essential.h>
import orion.engine;
import orion.editor;

int main(int argc, char* argv[])
{
    ored::editor_t editor;
    if (auto init_result = editor.initialize(); !init_result)
    {
        ORLOG_CRITICAL("Failed to initialize editor -> {}", init_result.error());
        return -1;
    }

    editor.run();
    editor.shutdown();

    return 0;
}
