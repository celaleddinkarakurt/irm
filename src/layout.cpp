#include "layout.hpp"

constexpr size_t TAB_WIDTH = 4;

Layout create_layout(const std::vector<std::string>& buffer)
{
    Layout layout;

    layout.line_number_width = std::to_string(buffer.size()).size();

    for (size_t i = 0; i < buffer.size(); i++)
    {
        const std::string& line = buffer[i];

        std::string screen_line;

        std::string line_number = std::to_string(i + 1);
        line_number.insert(0, layout.line_number_width - line_number.size(), ' ');
        line_number += " | ";

        screen_line += line_number;

        std::vector<size_t> cursor_columns;

        size_t screen_col = layout.line_number_width + 3;

        for (char c : line)
        {
            cursor_columns.push_back(screen_col);

            if (c == '\t')
            {
                size_t spaces = TAB_WIDTH - (screen_col % TAB_WIDTH);

                screen_line.append(spaces, ' ');
                screen_col += spaces;
            }
            else
            {
                screen_line += c;
                screen_col++;
            }
        }

        cursor_columns.push_back(screen_col);

        layout.lines.push_back(std::move(screen_line));
        layout.cursor_columns.push_back(std::move(cursor_columns));
    }

    return layout;
}

ScreenCursor Layout::buffer_to_screen(const Cursor& cursor) const
{
    return { cursor.row, cursor_columns[cursor.row][cursor.col] };
}