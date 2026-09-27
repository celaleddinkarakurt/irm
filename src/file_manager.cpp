#include "file_manager.hpp"
#include "app_state.hpp"
#include <filesystem>
#include <system_error>
#include <fstream>

namespace fs = std::filesystem;

FileStatus create_file(const std::string& fileName)
{
    fs::path path = fs::path(get_path()) / fileName;
    std::error_code ec;

    bool exists = fs::exists(path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    if (exists)
    {
        return ALREADY_EXISTS;
    }

    std::ofstream file(path);

    if (!file.is_open())
    {
        return CANNOT_CREATE;
    }

    return SUCCESS;
}

FileStatus create_folder(const std::string& folderName)
{
    fs::path path = fs::path(get_path()) / folderName;
    std::error_code ec;

    bool exists = fs::exists(path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    if (exists)
    {
        return ALREADY_EXISTS;
    }

    bool created = fs::create_directory(path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return CANNOT_CREATE;
    }

    if (!created)
    {
        return CANNOT_CREATE;
    }

    return SUCCESS;
}

FileStatus delete_path(const std::string& pathName)
{
    fs::path path = fs::path(get_path()) / pathName;
    std::error_code ec;

    bool exists = fs::exists(path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    if (!exists)
    {
        return NOT_FOUND;
    }

    if (fs::is_directory(path, ec))
    {
        fs::remove_all(path, ec);
    }
    else if (fs::is_regular_file(path, ec))
    {
        fs::remove(path, ec);
    }
    else
    {
        fs::remove(path, ec);
    }

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    return SUCCESS;
}

FileStatus rename_path(const std::string& oldName, const std::string& newName)
{
    fs::path old_path = fs::path(get_path()) / oldName;
    fs::path new_path = fs::path(get_path()) / newName;
    std::error_code ec;

    bool exists = fs::exists(old_path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    if (!exists)
    {
        return NOT_FOUND;
    }

    exists = fs::exists(new_path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    if (exists)
    {
        return ALREADY_EXISTS;
    }

    fs::rename(old_path, new_path, ec);

    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            return PERMISSION_DENIED;
        }

        return UNKNOWN_ERROR;
    }

    return SUCCESS;
}

FileStatus save_file(const std::string& path, const std::vector<std::string>& buffer, bool ends_with_new_line)
{
    std::ofstream file(path);

    if (!file.is_open())
    {
        return CANNOT_OPEN;
    }

    for (size_t i = 0; i < buffer.size(); i++)
    {
        file << buffer[i];

        if (!file)
        {
            return UNKNOWN_ERROR;
        }

        if (i < buffer.size() - 1 || ends_with_new_line)
        {
            file << "\n";

            if (!file)
            {
                return UNKNOWN_ERROR;
            }
        }
    }

    file.flush();

    if (!file)
    {
        return UNKNOWN_ERROR;
    }
    
    return SUCCESS;
}

FileReadResult read_file(const std::string& fileName)
{
    FileReadResult result;
    std::vector<std::string> content;

    fs::path path = fs::path(get_path()) / fileName;
    std::error_code ec;

    bool exists = fs::exists(path, ec);
    
    if (ec)
    {
        if (ec == std::errc::permission_denied)
        {
            result = { PERMISSION_DENIED, content, false };
            return result;
        }

        result = { UNKNOWN_ERROR, content, false };
        return result;
    }

    if (!exists)
    {
        result = { NOT_FOUND, content, false };
        return result;
    }

    std::ifstream file(path);

    if (!file.is_open())
    {
        result = { CANNOT_OPEN, content, false };
        return result;
    }

    std::string line;
    while (std::getline(file, line))
    {
        content.push_back(line);
    }

    bool ends_with_new_line = false;

    file.clear();
    file.seekg(0, std::ios::end);

    std::streampos file_size = file.tellg();

    if (file_size > 0)
    {
        file.seekg(-1, std::ios::end);

        char last_char;
        if (!file.get(last_char))
        {
            result = { UNKNOWN_ERROR, content, false };
            return result;
        }

        ends_with_new_line = (last_char == '\n');
    }
    
    result = { SUCCESS, content, ends_with_new_line };
    return result;
}