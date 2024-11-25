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


//Message structure for IPC
typedef struct {
	unsigned int id;       //airplane id
	    char command[100];      //command
	    int value;             //number to increment
} msg_struct;




//Constructors & Destructors
OperatorConsole::OperatorConsole(planeManager & manager) : manager(&manager), runningOperatorConsole(false) {}

OperatorConsole::~OperatorConsole() {
	// TODO Auto-generated destructor stub
}



//Start Thread
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

	        size_t firstSpace = input.find(' ');
	               size_t secondSpace = input.find(' ', firstSpace + 1);
	               if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
	                   cerr <<ERROR<< "Invalid command format. Use: <AircraftID> <Command> <Value>\n";
	                   continue;
	               }

	       string idStr = input.substr(0, firstSpace);
	                     string commandStr = input.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	                      string valueStr = input.substr(secondSpace + 1);
//

	        	//check id
	               int aircraftID,value;

	               try {
	            	   aircraftID = std::stoi(idStr);
	            	   value = std::stoi(valueStr);
	               } catch (const std::exception& e) {
	                   std::cerr << ERROR << "Error: Invalid Airplane ID or value. Please enter a valid int number!\n"<< ERROR;
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
	                          std::cerr << "Invalid command: " << commandStr << ".\n";
	                          continue;
	                      }

	               //2
	               //Send command
	               cout << "Processing command '" << commandStr << "' for Aircraft ID " << aircraftID << " Value:  "<<value<<endl;

	               sendCommand(aircraftID, commandStr, value);


	    }
}

void OperatorConsole::sendCommand(int aircraftID, const std::string& command, int value){

	//1.
	//Open the channel to connect to the Computer System (server)
	    int coid = name_open(CHANNEL_NAME, 0);
	    if (coid == -1) {
	        cerr << ERROR<< "Operator " << " failed to connect to channel: " << CHANNEL_NAME << endl<< ERROR;
	        return;
	    }

	    cout << ASTERISKS << "Operator " <<  " connected to server on channel: " << CHANNEL_NAME <<  endl;

	    // Prepare the message
	       msg_struct msg;
	       msg.id = aircraftID;
	       strncpy(msg.command, command.c_str(), sizeof(msg.command) - 1);
	       msg.command[sizeof(msg.command) - 1] = '\0';
	       msg.value = value;

	       // Send the message
	       msg_struct reply;
	       if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
	           perror("Error sending message");
	           name_close(coid);
	           return;
	       }

	       // Print the server's reply
	       std::cout << "OperatorConsole: Received reply: " << reply.command <<endl;

	       name_close(coid);

}

pthread_t OperatorConsole::getconsoleThread() const {
    return consoleThread;
}



//void OperatorConsole::handleCommands() {
//	 std::string input;
//
//	    while (true) {
////	        cout << ">> ";
//	        getline(cin, input);
//
//	        if (input == "exit") {
//	            std::cout << "Shutting down the system...\n";
//	            break;
//	        }
//
//	        // Parse the input to extract AircraftID and command string
//	        size_t spacePos = input.find(' ');
//	        if (spacePos == std::string::npos) {
//	            std::cerr <<ERROR<< "Invalid command format. Use: <AircraftID> <Command>\n"<<ERROR;
//	            continue;
//	        }
//
//	        string idStr = input.substr(0, spacePos);
//	        string commandStr = input.substr(spacePos + 1);
//
////
//
//	        	//check id
//	               int aircraftID;
//	               try {
//	                   aircraftID = std::stoi(idStr);
//	               } catch (const std::exception& e) {
//	                   std::cerr << ERROR << "Error: Invalid Airplane ID. Please enter a valid number.\n"<< ERROR;
//	                   continue;
//	               }
//	               //check id is in the PlaneManager / is in shared memory
//	               int isValidID = manager->checkIds(aircraftID);
//	               if (!isValidID) {
//	                   std::cerr << ERROR<< "Error: Aircraft ID " << aircraftID << " not found in the airspace.\n"<< ERROR;
//	                   continue;
//	               }
//
//	               //is in enum list of commands
//	               Command command = stringToCommand(commandStr);
//	               if (command == Command::INVALID) {
//	                   std::cerr << ERROR<< "Error: Command '" << commandStr << "' not valid. Valid commands are: pp, e, ca, cs, cp, ct.\n"<< ERROR;
//	                   continue;
//	               }
//
//
//	               //2
//	               //Send command
//	               std::cout << "Processing command '" << commandStr << "' for Aircraft ID " << aircraftID << ".\n";
//	               sendCommand(aircraftID, commandStr);
//
//
//	    }
//}
//
//void OperatorConsole::sendCommand(int aircraftID, const std::string& command, int value){
//
//	//1.
//	//Open the channel to connect to the Computer System (server)
//	    int coid = name_open(CHANNEL_NAME, 0);
//	    if (coid == -1) {
//	        cerr << ERROR<< "Operator " << " failed to connect to channel: " << CHANNEL_NAME << endl<< ERROR;
//	        return;
//	    }
//
//	    cout << ASTERISKS << "Operator " <<  " connected to server on channel: " << CHANNEL_NAME <<  endl;

	    //2
	    //Message
//	       msg_struct msg;
//	       msg.id = aircraftID;
//	       strncpy(msg.body, command.c_str(), sizeof(msg.body) - 1);
//	       msg.body[sizeof(msg.body) - 1] = '\0'; // Ensure null termination
//
//	       std::cout << "\nOperatorConsole: Sending command to ComputerSystem: " << msg.body <<ASTERISKS<< std::endl;
//
//	       //3
//	       //Structure for the server's reply
//	       msg_struct reply;
//
//	       //4
//	       //Send the message to the server and wait for a reply
//	       int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
//	       if (status == -1) {
//	           perror("MsgSend");
//	           name_close(coid);
//	           return;
//	       }
//
//
//	       std::cout << "\nOperatorConsole: Received reply from ComputerSystem: " << reply.body <<ASTERISKS<< std::endl;
//
//	       //Close connection
//	       name_close(coid);
//	       return;



	    //2
	    //Message
//	    msg_struct msg;
//	       msg.id = aircraftID;
//	       strncpy(msg.command, commandToString(command).c_str(), sizeof(msg.command) - 1);
//	       msg.command[sizeof(msg.command) - 1] = '\0'; // Ensure null-termination
//	       msg.value = value;
//
//	    // Send the message
//	       msg_struct reply;
//	       if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
//	           perror("Error sending message");
//	           name_close(coid);
//	           return;
//	       }
//
//	     //Reply from server
//	       std::cout << "OperatorConsole: Received reply: " << reply.command << std::endl;
//
//	       name_close(coid);


//}-

