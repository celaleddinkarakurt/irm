#ifndef APP_STATE_HPP
#define APP_STATE_HPP

#include <string>

enum class PathStatus
{
    SUCCESS,
    NOT_FOUND,
    NOT_DIRECTORY,
    PERMISSION_DENIED,
    UNKNOWN_ERROR
};

std::string get_path(void);
PathStatus change_path(const std::string& path);
std::string get_current_folder();

#endif