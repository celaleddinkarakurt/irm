#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <string>

enum KeyType
{
    CHARACTER,
    ENTER,
    BACKSPACE,
    SAVE,
    ESCAPE,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    NONE
};

enum class Color
{
    DEFAULT,
    GREEN
};

enum class SizeStatus
{
    SUCCESS,
    ERROR
};

struct Key
{
    KeyType type;
    char c;
};

struct TerminalSize
{
    SizeStatus status;
    size_t width;
    size_t height;
};

bool enable_raw_mode(void);
bool disable_raw_mode(void);
Key read_key(void);
TerminalSize get_terminal_size();
void move_cursor(size_t row, size_t col);
void clear_to_end_of_line(void);
void insert_line(void);
void delete_line(void);
void write_text(const std::string& text);
void write_colored_text(const std::string& text, Color color);
void clear_screen();

#endif