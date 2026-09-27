#include "app_state.hpp"
#include <vector>
#include <unistd.h>
#include <pwd.h>
#include <filesystem>

std::vector<std::string> initialize_path(void)
{
    struct passwd* pw = getpwuid(getuid());
    std::string rawPath = pw->pw_dir;
    std::vector<std::string> result;

    std::filesystem::path path(rawPath);
    for (const auto& part : path)
    {
        result.push_back(part.string());
    }

    return result;
}

std::vector<std::string> current_path = initialize_path();

std::string get_path(void)
{
    std::string path;

    if (current_path.empty())
    {
        path = "/";
    }
    else
    {
        for (const auto& part : current_path)
        {
            path += "/";
            path += part;
        }
    }
    
    return path;
}

void change_path(std::string path)
{

}