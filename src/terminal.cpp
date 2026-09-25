#include "terminal.hpp"
#include <termios.h>
#include <unistd.h>
#include <cerrno>
#include <poll.h>
#include <optional>

termios original_settings;
bool isEnable = false;

bool enable_raw_mode()
{
    if (isEnable) return false;

    termios raw;
    if (tcgetattr(STDIN_FILENO, &raw) == -1) return false;
    original_settings = raw;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return false;

    isEnable = true;
    return true;
}

bool disable_raw_mode()
{
    if (!isEnable) return false;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_settings) == -1) return false;

    isEnable = false;
    return true;
}

std::optional<char> read_byte()
{
    char c;

    if (read(STDIN_FILENO, &c, 1) != 1) return std::nullopt;

    return c;
}

bool input_available()
{
    pollfd pfd = { STDIN_FILENO, POLLIN, 0 };

    int ret = poll(&pfd, 1, 50);

    return ret > 0 && (pfd.revents & POLLIN) != 0;
}

Key read_key()
{
    Key key;

    auto byte = read_byte();
    if (!byte)
    {
        key.type = NONE;
        return key;
    }

    if (*byte != '\x1b')
    {
        switch (*byte)
        {
            case '\n':
                key.type = ENTER;
                break;
            case '\b':
            case '\x7f':
                key.type = BACKSPACE;
                break;
            default:
                key.type = CHARACTER;
        }
        
        key.c = *byte;
        return key;
    }

    if (input_available())
    {
        auto next_byte = read_byte();
        if(!next_byte)
        {
            key.type = NONE;
            return key;
        }

        if (*next_byte == '[' || *next_byte == 'O')
        {
            if (!input_available())
            {
                key.type = NONE;
                return key;
            }

            next_byte = read_byte();
            if (!next_byte)
            {
                key.type = NONE;
                return key;
            }

            switch (*next_byte)
            {
                case 'A':
                    key.type = ARROW_UP;
                    break;
                case 'B':
                    key.type = ARROW_DOWN;
                    break;
                case 'C':
                    key.type = ARROW_RIGHT;
                    break;
                case 'D':
                    key.type = ARROW_LEFT;
                    break;
                default:
                    key.type = NONE;
                    break;
            }

            return key;
        }
            
        key.type = NONE;
        return key;
    }

    key.type = ESCAPE;
    return key;
}
