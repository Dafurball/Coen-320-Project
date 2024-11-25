#include "OperatorConsole.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>
#include <thread>
#include <string>

using namespace std;
std::mutex inputMutex;

#define CHANNEL_NAME "ComputerSystemServer"

#define ASTERISKS "\n*************************************************************************************************\n"
#define ERROR "\n\n/////////////////////////////////////////////////////////////\n\n"

typedef struct {
    unsigned int id;   // Identifier for the message (e.g., Aircraft ID)
    char body[100];    // Content of the message (e.g., command or data)
} msg_struct;


OperatorConsole::OperatorConsole(planeManager & manager) : manager(&manager), runningOperatorConsole(false) {}

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
	 std::string input;

	    while (true) {
//	        cout << ">> ";
	        getline(cin, input);

	        if (input == "exit") {
	            std::cout << "Shutting down the system...\n";
	            break;
	        }

	        // Parse the input to extract AircraftID and command string
	        size_t spacePos = input.find(' ');
	        if (spacePos == std::string::npos) {
	            std::cerr <<ERROR<< "Invalid command format. Use: <AircraftID> <Command>\n"<<ERROR;
	            continue;
	        }

	        string idStr = input.substr(0, spacePos);
	        string commandStr = input.substr(spacePos + 1);

//	        // Convert string to Command enum
//	        Command command = stringToCommand(commandStr);

//	                //Checking if command is in enum
//	                if (command == Command::INVALID) {
//	                    std::cerr << "Command '" << commandStr << "' not recognized. Valid commands are: pp, e, ca, cs, cp, ct.\n";
//	                    continue;
//	                } else {
//	                    std::cout << "Command '" << commandStr << "' recognized.\n";
//	                }
//
//	                try {
//	                    int aircraftID = std::stoi(idStr); // Convert Aircraft ID to an integer
//	                    int answer =  manager->checkIds(aircraftID);
//	                    // Process the command
//	                    sendCommand(aircraftID, commandStr);
//	                } catch (const std::exception& e) {
//	                    std::cerr << "Error: Invalid Aircraft ID. Please enter a valid number.\n";
//	                }



	        	//check id
	               int aircraftID;
	               try {
	                   aircraftID = std::stoi(idStr);
	               } catch (const std::exception& e) {
	                   std::cerr << ERROR << "Error: Invalid Airplane ID. Please enter a valid number.\n"<< ERROR;
	                   continue;
	               }
	               //check id is in the PlaneManager / is in shared memory
	               int isValidID = manager->checkIds(aircraftID);
	               if (!isValidID) {
	                   std::cerr << ERROR<< "Error: Aircraft ID " << aircraftID << " not found in the airspace.\n"<< ERROR;
	                   continue;
	               }

	               //is in enum list of commands
	               Command command = stringToCommand(commandStr);
	               if (command == Command::INVALID) {
	                   std::cerr << ERROR<< "Error: Command '" << commandStr << "' not valid. Valid commands are: pp, e, ca, cs, cp, ct.\n"<< ERROR;
	                   continue;
	               }


	               //2
	               //Send command
	               std::cout << "Processing command '" << commandStr << "' for Aircraft ID " << aircraftID << ".\n";
	               sendCommand(aircraftID, commandStr);


	    }
}

void OperatorConsole::sendCommand(int aircraftID, const std::string& command){

	//1.
	//Open the channel to connect to the Computer System (server)
	    int coid = name_open(CHANNEL_NAME, 0);
	    if (coid == -1) {
	        cerr << ERROR<< "Operator " << " failed to connect to channel: " << CHANNEL_NAME << endl<< ERROR;
	        return;
	    }

	    cout << ASTERISKS << "Operator " <<  " connected to server on channel: " << CHANNEL_NAME <<  endl;

	    //2
	    //Message
	       msg_struct msg;
	       msg.id = aircraftID;
	       strncpy(msg.body, command.c_str(), sizeof(msg.body) - 1);
	       msg.body[sizeof(msg.body) - 1] = '\0'; // Ensure null termination

	       std::cout << "\nOperatorConsole: Sending command to ComputerSystem: " << msg.body <<ASTERISKS<< std::endl;

	       //3
	       //Structure for the server's reply
	       msg_struct reply;

	       //4
	       //Send the message to the server and wait for a reply
	       int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
	       if (status == -1) {
	           perror("MsgSend");
	           name_close(coid);
	           return;
	       }


	       std::cout << "\nOperatorConsole: Received reply from ComputerSystem: " << reply.body <<ASTERISKS<< std::endl;

	       //Close connection
	       name_close(coid);
	       return;
}

pthread_t OperatorConsole::getconsoleThread() const {
    return consoleThread;
}


