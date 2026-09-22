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

    Cursor& cursor = editor.cursor;
    auto& buffer = editor.buffer;

    
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
                buffer[cursor.row].insert(cursor.col, std::string(1, key.c));
                cursor.col++;

                break;
            case ENTER:
                buffer.insert(buffer.begin() + cursor.row + 1, buffer[cursor.row].substr(cursor.col));
                buffer[cursor.row].erase(cursor.col);

                cursor.row++;
                cursor.col = 0;

                break;
            case BACKSPACE:
                if (cursor.col > 0)
                {
                    cursor.col--;
                    buffer[cursor.row].erase(cursor.col, 1);

                    break;
                }

                if (cursor.col == 0 && cursor.row != 0)
                {
                    cursor.row--;
                    cursor.col = buffer[cursor.row].size();

                    buffer[cursor.row] += buffer[cursor.row + 1];
                    buffer.erase(buffer.begin() + cursor.row + 1);
                }

                break;
            case ESCAPE:
                running = false;
                break;
            case ARROW_UP:
                if (cursor.row > 0)
                {
                    cursor.row--;

                    if (buffer[cursor.row].size() < cursor.col)
                        cursor.col = buffer[cursor.row].size();
                }

                break;
            case ARROW_DOWN:
                if (cursor.row < (buffer.size() - 1))
                {
                    cursor.row++;
                    
                    if (buffer[cursor.row].size() < cursor.col)
                        cursor.col = buffer[cursor.row].size();
                }

                break;
            case ARROW_LEFT:
                if (cursor.col > 0) 
                    cursor.col--;    

                break;
            case ARROW_RIGHT:
                if (cursor.col < buffer[cursor.row].size()) 
                    cursor.col++;

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