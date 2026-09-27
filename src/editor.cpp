#include "editor.hpp"
#include "mode.hpp"
#include "terminal.hpp"
#include "renderer.hpp"
#include "file_manager.hpp"
#include <iostream>

Editor::Editor(Cursor cursor, std::vector<std::string> buffer, std::string file_path, bool ends_with_new_line)
:   cursor(cursor)
,   buffer(std::move(buffer))
,   file_path(std::move(file_path))
,   ends_with_new_line(ends_with_new_line)
{
    if (this->buffer.empty()) this->buffer.push_back("");
}

void run_editor(Mode& mode)
{
    Editor& editor = std::get<Editor>(mode.data);
    Screen screen;

    TerminalSize t_size = get_terminal_size();

    if (t_size.status == SizeStatus::ERROR)
    {
        std::cout << "An error occurred.\n";

        mode.type = ModeType::COMMAND;
        mode.data = std::monostate{};

        return;
    }

    screen.width = t_size.width;
    screen.height = t_size.height;

    Cursor& cursor = editor.cursor;
    auto& buffer = editor.buffer;
    
    if (!enable_raw_mode())
    {
        std::cout << "An error occurred.\n";

        mode.type = ModeType::COMMAND;
        mode.data = std::monostate{};

        return;
    }

    clear_screen();

    bool running = true;
    while (running)
    {
        if (screen.needs_render) render(editor, screen);

        Key key = read_key();
        switch (key.type)
        {
            case CHARACTER:
            {
                screen.dirty = { cursor.row, cursor.col };

                buffer[cursor.row].insert(cursor.col, std::string(1, key.c));
                cursor.col++;

                if (cursor.row == buffer.size() - 1) editor.ends_with_new_line = false;

                screen.needs_render = true;
                break;
            }
            case ENTER:
            {
                screen.dirty = { cursor.row, cursor.col };

                bool was_last_line = cursor.row == buffer.size() - 1;

                buffer.insert(buffer.begin() + cursor.row + 1, buffer[cursor.row].substr(cursor.col));
                buffer[cursor.row].erase(cursor.col);

                cursor.row++;
                cursor.col = 0;

                if (was_last_line)
                {
                    editor.ends_with_new_line = true;
                }

                screen.needs_render = true;
                break;
            }
            case BACKSPACE:
            {
                if (cursor.col > 0)
                {
                    cursor.col--;

                    screen.dirty = { cursor.row, cursor.col };

                    buffer[cursor.row].erase(cursor.col, 1);

                    screen.needs_render = true;
                    break;
                }

                if (cursor.col == 0 && cursor.row != 0)
                {
                    bool was_last_line = cursor.row == buffer.size() - 1;

                    cursor.row--;
                    cursor.col = buffer[cursor.row].size();

                    screen.dirty = { cursor.row, cursor.col };

                    buffer[cursor.row] += buffer[cursor.row + 1];
                    buffer.erase(buffer.begin() + cursor.row + 1);

                    if (was_last_line)
                    {
                        editor.ends_with_new_line = false;
                    }

                    screen.needs_render = true;
                }

                break;
            }
            case SAVE:
            {
                FileStatus status = save_file(editor.file_path, editor.buffer, editor.ends_with_new_line);

                break;
            }
            case ESCAPE:
                running = false;
                break;
            case ARROW_UP:
                if (cursor.row > 0)
                {
                    cursor.row--;

                    if (buffer[cursor.row].size() < cursor.col)
                        cursor.col = buffer[cursor.row].size();

                    screen.needs_render = true;
                }

                break;
            case ARROW_DOWN:
                if (cursor.row < (buffer.size() - 1))
                {
                    cursor.row++;
                    
                    if (buffer[cursor.row].size() < cursor.col)
                        cursor.col = buffer[cursor.row].size();

                    screen.needs_render = true;
                }

                break;
            case ARROW_LEFT:
                if (cursor.col > 0)
                {
                    cursor.col--;
                    screen.needs_render = true;
                } 

                break;
            case ARROW_RIGHT:
                if (cursor.col < buffer[cursor.row].size())
                {
                   cursor.col++;
                   screen.needs_render = true;
                }
                
                break;
            default:
                break;
        }

        if (cursor.row < screen.row_offset)
        {
            screen.row_offset = cursor.row;
            screen.lines.clear();
            screen.needs_render = true;
        }
        else if (cursor.row >= screen.row_offset + screen.height)
        {
            screen.row_offset = cursor.row - screen.height + 1;
            screen.lines.clear();
            screen.needs_render = true;
        }
    }

    if (!disable_raw_mode())
    {
        std::cout << "An error occurred.\n";

        return;
    }

    clear_screen();

    mode.type = ModeType::COMMAND;
    mode.data = std::monostate{};
}