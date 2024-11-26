
#include <string>
#include <unordered_map>
#include <iostream>

#include "Command.h"

using namespace std;

Command stringToCommand(const string& command) {
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

