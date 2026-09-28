#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include "mode.hpp"

enum CommandTypes
{
    CREATE,
    OPEN,
    DELETE,
    RENAME,
    MOVE,
    IMAGE,
    EXIT,
    UNKNOWN
};

enum CommandStatus
{
    OK,
    UNKNOWN_COMMAND,
    INVALID_ARGUMENT_COUNT,
    INVALID_ARGUMENT
};

typedef struct
{
    CommandTypes commandType;
    std::vector<std::string> commands;
} Command;

typedef struct 
{
    CommandStatus status;
    Command cmd;
} CommandResult;

void run_command_cycle(bool& running, Mode& mode);

#endif