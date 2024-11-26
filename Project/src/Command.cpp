
#include <string>
#include <unordered_map>
#include <iostream>

#include "Command.h"

Command stringToCommand(const std::string& command) {
    static const std::unordered_map<std::string, Command> commandMap = {
        {"pp", Command::PP},
        {"ca", Command::CA},
        {"cs", Command::CS},
        {"cp", Command::CP},
        {"ct", Command::CT}
    };

    auto it = commandMap.find(command);
    if (it != commandMap.end()) {
        return it->second;
    }
    return Command::INVALID;
}

