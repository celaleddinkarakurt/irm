#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <string>
#include <vector>

struct Mode;

struct Cursor
{
    size_t row;
    size_t col;    
};

struct Editor
{
    Cursor cursor;
    std::vector<std::string> buffer;

    Editor(Cursor cursor, std::vector<std::string> buffer);
};

void run_editor(Mode& mode);

#endif