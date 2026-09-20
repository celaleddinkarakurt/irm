#ifndef TERMINAL_HPP
#define TERMINAL_HPP

enum KeyType
{
    CHARACTER,
    ENTER,
    BACKSPACE,
    ESCAPE,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    NONE
};

struct Key
{
    KeyType type;
    char c;
};

bool enable_raw_mode();
bool disable_raw_mode();
Key read_key();

#endif