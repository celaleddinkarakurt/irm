#include "editor.hpp"
#include "mode.hpp"
#include "terminal.hpp"
#include <iostream>

Editor::Editor(Cursor cursor, std::vector<std::string> buffer)
:   cursor(cursor)
,   buffer(std::move(buffer))
{
    if (this->buffer.empty()) this->buffer.push_back("");
}

void run_editor(Mode& mode)
{
    Editor& editor = std::get<Editor>(mode.data);
    
    if (!enable_raw_mode())
    {
        std::cout << "An error occurred.\n";

        mode.type = ModeType::COMMAND;
        mode.data = std::monostate{};

        return;
    }

    bool running = true;
    while (running)
    {
        Key key = read_key();

        switch (key.type)
        {
            case CHARACTER:
                

                break;
            case ENTER:
                break;
            case BACKSPACE:
                break;
            case ESCAPE:
                running = false;
                break;
            case ARROW_UP:
                if (editor.cursor.row > 0) editor.cursor.row--;

                break;
            case ARROW_DOWN:
                if (editor.cursor.row < editor.buffer.size()) editor.cursor.row++;

                break;
            case ARROW_LEFT:
                if (editor.cursor.col > 0) editor.cursor.col--;    

                break;
            case ARROW_RIGHT:
                if (editor.cursor.col < editor.buffer.size()) editor.cursor.col++;    

                break;
            default:
                break;
        }
    }

    if (!disable_raw_mode())
    {
        std::cout << "An error occurred.\n";

        return;
    }

    mode.type = ModeType::COMMAND;
    mode.data = std::monostate{};
}