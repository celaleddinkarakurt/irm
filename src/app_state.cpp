#include "app_state.hpp"
#include <vector>
#include <unistd.h>
#include <pwd.h>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

std::vector<std::string> initialize_path(void)
{
    struct passwd* pw = getpwuid(getuid());
    std::string rawPath = pw->pw_dir;
    std::vector<std::string> result;

    fs::path path(rawPath);
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

PathStatus change_path(const std::string& path)
{
    fs::path target;

    if (path.empty()) return PathStatus::NOT_FOUND;

    if (path[0] == '/')
    {
        target = fs::path(path);
    }
    else
    {
        target = fs::path(get_path()) / path;
    }

    target = fs::weakly_canonical(target);

    std::error_code ec;

    if (!fs::exists(target, ec))
    {
        if (ec == std::errc::permission_denied)
            return PathStatus::PERMISSION_DENIED;

        if (ec)
            return PathStatus::UNKNOWN_ERROR;

        return PathStatus::NOT_FOUND;
    }

    if (!fs::is_directory(target, ec))
    {
        if (ec == std::errc::permission_denied)
            return PathStatus::PERMISSION_DENIED;

        if (ec)
            return PathStatus::UNKNOWN_ERROR;

        return PathStatus::NOT_DIRECTORY;
    }

    if (ec)
    {
        if (ec == std::errc::permission_denied)
            return PathStatus::PERMISSION_DENIED;

        return PathStatus::UNKNOWN_ERROR;
    }

    current_path.clear();

    for (const auto& part : target)
    {
        current_path.push_back(part.string());
    }

    return PathStatus::SUCCESS;
}

std::string get_current_folder()
{
    return current_path.back();
}