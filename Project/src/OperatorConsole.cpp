#include "OperatorConsole.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>
#include <thread>
#include <string>

using namespace std;

#define CHANNEL_NAME "ComputerSystemServer"

#define ASTERISKS "\n*************************************************************\n"

typedef struct {
    unsigned int id;   // Identifier for the message (e.g., Aircraft ID)
    char body[100];    // Content of the message (e.g., command or data)
} msg_struct;


OperatorConsole::OperatorConsole() : runningOperatorConsole(false) {}

OperatorConsole::~OperatorConsole() {
	// TODO Auto-generated destructor stub
}


void OperatorConsole::startOperatorConsoleThread() {
    runningOperatorConsole = true;
    pthread_create(&consoleThread, nullptr, processCommands, this);
}

void* OperatorConsole::processCommands(void* arg) {
    OperatorConsole* console = static_cast<OperatorConsole*>(arg);
    console->handleCommands();
    return nullptr;
}

void OperatorConsole::handleCommands() {
    std::string command;

    while (runningOperatorConsole) {
        std::cout << ">> "; // Prompt for user input
        std::getline(std::cin, command);

        if (command == "exit") {
            std::cout << "Exiting Operator Console...\n";
            runningOperatorConsole = false;
            break;
        }

        // Parse the command
        size_t spacePos = command.find(' ');
        if (spacePos == std::string::npos) {
            std::cerr << "Invalid command format. Use: <AircraftID> <Command>\n";
            continue;
        }

        std::string idStr = command.substr(0, spacePos);
        std::string action = command.substr(spacePos + 1);

        try {
            int aircraftID = std::stoi(idStr); // Convert Aircraft ID to an integer
            sendCommand(aircraftID, action);
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid Aircraft ID.\n";
        }
    }
}



void OperatorConsole::sendCommand(int aircraftID, const std::string& command){

	//1.
	// Open the channel to connect to the Computer System (server)
	    int coid = name_open(CHANNEL_NAME, 0);
	    if (coid == -1) {
	        cerr << "Operator " << " failed to connect to channel: " << CHANNEL_NAME << endl;
	        return;
	    }

	    cout << ASTERISKS << "Operator " <<  " connected to server on channel: " << CHANNEL_NAME << ASTERISKS<< endl;

	    // 2. Prepare the message structure
	       msg_struct msg;
	       msg.id = aircraftID;
	       strncpy(msg.body, command.c_str(), sizeof(msg.body) - 1);
	       msg.body[sizeof(msg.body) - 1] = '\0'; // Ensure null termination

	       std::cout << ASTERISKS<<"OperatorConsole: Sending command to ComputerSystem: " << msg.body <<ASTERISKS<< std::endl;

	       // 3. Structure for the server's reply
	       msg_struct reply;

	       // 4. Send the message to the server and wait for a reply
	       int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
	       if (status == -1) {
	           perror("MsgSend");
	           name_close(coid);
	           return;
	       }

	       // 5. Display the server's reply
	       std::cout <<ASTERISKS<< "OperatorConsole: Received reply from ComputerSystem: " << reply.body <<ASTERISKS<< std::endl;

	       // 6. Close the connection to the server
	       name_close(coid);
	       return;
}

pthread_t OperatorConsole::getconsoleThread() const {
    return consoleThread;
}



//void OperatorConsole::processCommands() {
//    std::string command;
//    while (runningOperatorConsole) {
//        std::cout << ">> ";
//        std::getline(std::cin, command);
//
//        if (command == "exit") {
//            std::cout << "Exiting Operator Console...\n";
//            runningOperatorConsole = false;
//            break;
//        }1
//
//        // Parse and process commands
//        size_t spacePos = command.find(' ');
//        if (spacePos == std::string::npos) {
//            std::cerr << "Invalid command format. Use: <AircraftID> <Command>\n";
//            continue;
//        }
//
//        std::string idStr = command.substr(0, spacePos);
//        std::string action = command.substr(spacePos + 1);
//
//        try {
//            int aircraftID = std::stoi(idStr);
//            sendCommand(aircraftID, action); // Use the existing sendCommand method
//        } catch (const std::exception& e) {
//            std::cerr << "Error: Invalid Aircraft ID.\n";
//        }
//    }
//}
