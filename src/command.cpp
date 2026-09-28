#include "command.hpp"
#include "file_manager.hpp"
#include "editor.hpp"
#include "app_state.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

Command parse_input(const std::string& input)
{
    std::string commandType;
    std::string word;
    std::vector<std::string> commands;

    Command cmd;

    std::stringstream ss(input);
    if (ss >> commandType)
    {
        while (ss >> word)
        {
            commands.push_back(word);
        }
    }

    if (commandType == "create")
    {
        cmd = { CommandTypes::CREATE, commands};
    }
    else if (commandType == "open")
    {
        cmd = { CommandTypes::OPEN, commands };
    }
    else if (commandType == "delete")
    {
        cmd = { CommandTypes::DELETE, commands};
    }
    else if (commandType == "rename")
    {
        cmd = { CommandTypes::RENAME, commands };
    }
    else if (commandType == "move")
    {
        cmd = { CommandTypes::MOVE, commands };
    }
    else if (commandType == "image")
    {
        cmd = { CommandTypes::IMAGE, commands };
    }
    else if (commandType == "exit")
    {
        cmd = { CommandTypes::EXIT, commands };
    }
    else
    {
        cmd = { CommandTypes::UNKNOWN, commands};
    }

    return cmd;
}

CommandResult check_command(const Command& cmd)
{
    CommandResult cmdResult;

    switch (cmd.commandType)
    {
        case CommandTypes::CREATE:
            if (cmd.commands.size() != 2)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else if (cmd.commands[0] != "file" && cmd.commands[0] != "folder")
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT, cmd };
            }
            else{
                cmdResult = { CommandStatus::OK, cmd};
            }

            break;
        case CommandTypes::OPEN:
            if (cmd.commands.size() != 1)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else
            {
                cmdResult = { CommandStatus::OK, cmd };
            }

            break;
        case CommandTypes::DELETE:
            if (cmd.commands.size() != 1)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else
            {
                cmdResult = { CommandStatus::OK, cmd };
            }

            break;
        case CommandTypes::RENAME:
            if (cmd.commands.size() != 2)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else
            {
                cmdResult = { CommandStatus::OK, cmd };
            }

            break;
        case CommandTypes::MOVE:
            if (cmd.commands.size() != 1)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else
            {
                cmdResult = { CommandStatus::OK, cmd };
            }
        case CommandTypes::IMAGE:
            if (cmd.commands.size() != 1)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else
            {
                cmdResult = { CommandStatus::OK, cmd };
            }

            break;
        case CommandTypes::EXIT:
            if (cmd.commands.size() != 0)
            {
                cmdResult = { CommandStatus::INVALID_ARGUMENT_COUNT, cmd };
            }
            else
            {
                cmdResult = { CommandStatus::OK, cmd };
            }

            break;
        default:
            cmdResult = { CommandStatus::UNKNOWN_COMMAND, cmd};    

            break;
    }

    return cmdResult;
}

void execute_command(const Command& cmd, Mode& mode, bool& running)
{
    FileStatus result;

    switch (cmd.commandType)
    {
        case CommandTypes::CREATE:
            if (cmd.commands[0] == "file")
            {
                result = create_file(cmd.commands[1]);
            }
            else if (cmd.commands[0] == "folder")
            {
                result = create_folder(cmd.commands[1]);
            }

            switch (result)
            {
                case FileStatus::SUCCESS:
                    std::cout << "Successfully created!\n";
                    break;
                case FileStatus::ALREADY_EXISTS:
                    std::cout << "'" << cmd.commands[1] << "' already exists.\n";
                    break;
                case FileStatus::PERMISSION_DENIED:
                    std::cout << "Permission denied.\n";
                    break;
                case FileStatus::CANNOT_CREATE:
                    std::cout << "'" << cmd.commands[1] <<"' cannot created.\n";
                    break;
                default:
                    std::cout << "An error occurred.\n";
                    break;
            }

            break;
        case CommandTypes::OPEN:
        {
            FileReadResult read_result = read_file(cmd.commands[0]);

            switch (read_result.status)
            {
                case FileStatus::SUCCESS:
                {
                    std::string file_path = get_path() + "/" + cmd.commands[0];
                    mode = { ModeType::EDITOR, Editor{{ 0, 0 }, std::move(read_result.content), file_path, read_result.ends_with_new_line}};

                    break;
                }
                case FileStatus::NOT_FOUND:
                    std::cout << "File not found.\n";
                    break;
                case FileStatus::PERMISSION_DENIED:
                    std::cout << "Permission denied.\n";
                    break;         
                default:
                    std::cout << "An error occurred.\n";
                    break;
            }
                
            break;
        }
        case CommandTypes::DELETE:
            result = delete_path(cmd.commands[0]);

            switch (result)
            {
                case FileStatus::SUCCESS:
                    std::cout << "Successfully deleted!\n";
                    break;
                case FileStatus::NOT_FOUND:
                    std::cout << "'" << cmd.commands[0] << "' not found.\n";
                    break;
                case FileStatus::PERMISSION_DENIED:
                    std::cout << "Permission denied.\n";
                    break;
                default:
                    std::cout << "An error occurred.\n";
                    break;
            }

            break;
        case CommandTypes::RENAME:
            result = rename_path(cmd.commands[0], cmd.commands[1]);

            switch (result)
            {
                case FileStatus::SUCCESS:
                    std::cout << "Successfully changed!\n";
                    break;
                case FileStatus::NOT_FOUND:
                    std::cout << "'" << cmd.commands[0] << "' not found.\n";
                    break;
                case FileStatus::ALREADY_EXISTS:
                    std::cout << "'" << cmd.commands[1] << "' already exists.\n";
                    break;
                case FileStatus::PERMISSION_DENIED:
                    std::cout << "Permission denied.\n";
                    break;
                default:
                    std::cout << "An error occurred.\n";
                    break;
            }

            break;
        case CommandTypes::MOVE:
        {
            PathStatus result = change_path(cmd.commands[0]);

            switch (result)
            {
                case PathStatus::SUCCESS:
                    break;
                case PathStatus::NOT_FOUND:
                    std::cout << "'" << cmd.commands[0] << "' not found.\n";
                    break;
                case PathStatus::NOT_DIRECTORY:
                    std::cout << "'" << cmd.commands[0] << "' not a directory.\n";
                    break;
                case PathStatus::PERMISSION_DENIED:
                    std::cout << "Permission denied.\n";
                    break;
                default:
                    std::cout << "An error occurred.\n";
                    break;
            }

            break;
        }
        case CommandTypes::IMAGE:
                

            break;
        case CommandTypes::EXIT:
            running = false;
            break;
        default:
            break;
    }
}

void handle_command_result(const CommandResult& cmdResult, Mode& mode, bool& running)
{
    switch (cmdResult.status)
    {
        case CommandStatus::OK:
            execute_command(cmdResult.cmd, mode, running);
            break;
        case CommandStatus::UNKNOWN_COMMAND:
            std::cout << "Unknown command.\n";
            break;
        case CommandStatus::INVALID_ARGUMENT_COUNT:
            std::cout << "Invalid argument count: " << cmdResult.cmd.commands.size() << '\n';
            break;
        case CommandStatus::INVALID_ARGUMENT:
            std::cout << "Invalid argument: '" << cmdResult.cmd.commands[0] << "'\n";
            break;
        default:
            break;
    }
}

void run_command_cycle(bool& running, Mode& mode)
{
    std::string input;

    std::cout << "\e[35mirm(" << get_current_folder() << ")> \e[0m";
    std::getline(std::cin, input);

    Command cmd = parse_input(input);
    CommandResult cmdResult = check_command(cmd);

    handle_command_result(cmdResult, mode, running);
}