#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "layout.hpp"
#include "editor.hpp"
#include <string>
#include <vector>

enum class UIState
{
    EDITOR,
    COMMAND_LINE,
    STATUS_BAR
};

struct DirtyRegion
{
    size_t row = 0;
    size_t col = 0;
};

struct Screen
{
    size_t width = 0;
    size_t height = 0;

    size_t row_offset = 0;
    size_t col_offset = 0;

    size_t line_number_width = 0;

    ScreenCursor cursor;

    UIState ui_state = UIState::EDITOR;

    bool needs_render = true;

    std::vector<std::string> lines;

    DirtyRegion dirty;
};

void render(const Editor& editor, Screen& screen);

#endif