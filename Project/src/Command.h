
#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_


#include <string>
#include <unordered_map>

//Enum for Commands
//Here are all the commands available to Controller
enum class Command {
    PP,	//print airplane information
    CA,	//change altitude
    CS,	//change speed
    CP,	//change position
    CT,	//change time between collision check
    INVALID
};


Command stringToCommand(const std::string& command);

#endif /* SRC_COMMAND_H_ */
