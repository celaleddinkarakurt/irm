#include "application.hpp"
#include "mode.hpp"
#include "command.hpp"
#include "editor.hpp"
#include "image_viewer.hpp"
#include <iostream>

int run_application()
{
    std::cout << "\x1b[3J\x1b[2J\x1b[H" << std::flush;

    Mode mode = { ModeType::COMMAND, std::monostate{} };
    
    bool running = true;
    while (running)
    {
        switch (mode.type)
        {
            case ModeType::COMMAND:
                run_command_cycle(running, mode);
                break;
            case ModeType::EDITOR:
                run_editor(mode);
                break;
            case ModeType::IMAGE:
                break;
        }
    }

    return 0;
}
