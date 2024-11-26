
#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_


#include <string>
#include <unordered_map>

//Enum for Commands
enum class Command {
    PP,	//printPlane
    CA,	//change altitude
    CS,	//change speed
    CP,	//change position
    CT,	//change time between collision
    INVALID
};


Command stringToCommand(const std::string& command);

#endif /* SRC_COMMAND_H_ */
