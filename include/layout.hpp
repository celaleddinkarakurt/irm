#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "editor.hpp"
#include <string>
#include <vector>

struct ScreenCursor
{
    size_t row = 0;
    size_t col = 0;
};

struct Layout
{
    std::vector<std::string> lines;
    std::vector<std::vector<size_t>> cursor_columns;

    size_t line_number_width = 0;

    ScreenCursor buffer_to_screen(const Cursor& cursor) const;
};

Layout create_layout(const std::vector<std::string>& buffer);

#endif