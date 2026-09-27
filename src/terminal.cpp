#include "terminal.hpp"
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>
#include <poll.h>
#include <optional>
#include <string>
#include <cstring>

termios original_settings;
bool isEnable = false;

bool enable_raw_mode(void)
{
    if (isEnable) return false;

    termios raw;
    if (tcgetattr(STDIN_FILENO, &raw) == -1) return false;
    original_settings = raw;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return false;

    isEnable = true;
    return true;
}

bool disable_raw_mode(void)
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

Key read_key(void)
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
            case '\x13':
                key.type = SAVE;
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

TerminalSize get_terminal_size(void)
{
    winsize ws{};
    TerminalSize size;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        size.status = SizeStatus::ERROR;
        return size;
    }

    size.status = SizeStatus::SUCCESS;
    size.width = ws.ws_col;
    size.height = ws.ws_row;
    return size;
}

void move_cursor(size_t row, size_t col)
{
    row += 1;
    col += 1;

    std::string cmd = "\x1b[" + std::to_string(row) + ";" + std::to_string(col) + "H";
    write(STDOUT_FILENO, cmd.data(), cmd.size());
}

void clear_to_end_of_line(void)
{
    const char cmd[] = "\x1b[0K";

    write(STDOUT_FILENO, cmd, sizeof(cmd) - 1);
}

void insert_line(void)
{
    const char cmd[] = "\x1b[L";

    write(STDOUT_FILENO, cmd, sizeof(cmd) - 1);
}

void delete_line(void)
{
    const char cmd[] = "\x1b[M";

    write(STDOUT_FILENO, cmd, sizeof(cmd) - 1);
}

void write_text(const std::string& text)
{
    write(STDOUT_FILENO, text.data(), text.size());
}

void write_colored_text(const std::string& text, Color color)
{
    const char* color_code = "";

    switch (color)
    {
        case Color::GREEN:
            color_code = "\x1b[32m";
            break;
        case Color::DEFAULT:
            color_code = "\x1b[0m";
            break;
    }

    write(STDOUT_FILENO, color_code, strlen(color_code));
    write(STDOUT_FILENO, text.data(), text.size());
    write(STDOUT_FILENO, "\x1b[0m", 4);
}

void clear_screen()
{
    const char cmd[] = "\x1b[2J\x1b[3J\x1b[H";
    write(STDOUT_FILENO, cmd, sizeof(cmd) - 1);
}