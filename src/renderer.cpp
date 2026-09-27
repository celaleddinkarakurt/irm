#include "renderer.hpp"
#include "terminal.hpp"
#include "layout.hpp"
#include <algorithm>
#include <utility>

void render(const Editor& editor, Screen& screen)
{
    Layout layout = create_layout(editor.buffer);
    std::vector<std::string> visible_lines(screen.height, "");

    bool layout_changed = layout.line_number_width != screen.line_number_width;

    for (size_t screen_row = 0; screen_row < screen.height; screen_row++)
    {
        size_t buffer_row = screen.row_offset + screen_row;

        if (buffer_row < layout.lines.size())
            visible_lines[screen_row] = layout.lines[buffer_row];
    }

    for (size_t screen_row = 0; screen_row < screen.height; screen_row++)
    {
        if (layout_changed || screen_row >= screen.lines.size() || visible_lines[screen_row] != screen.lines[screen_row])
        {
            size_t prefix_length = layout.line_number_width + 3;

            move_cursor(screen_row, 0);

            const std::string& line = visible_lines[screen_row];

            if (line.size() >= prefix_length)
            {
                write_colored_text(line.substr(0, prefix_length), Color::GREEN);
                write_text(line.substr(prefix_length));
            }
            else
            {
                write_text(line);
            }

            clear_to_end_of_line();
        }
    }

    screen.lines = std::move(visible_lines);
    screen.line_number_width = layout.line_number_width;
    screen.needs_render = false;

    ScreenCursor cursor = layout.buffer_to_screen(editor.cursor);

    move_cursor(cursor.row - screen.row_offset, cursor.col);
    screen.cursor = cursor;
}
