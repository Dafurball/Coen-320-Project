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
    unsigned int id;        // Airplane ID
    char command[100];      // Command string
    int valueX;             // Additional value (X)
    int valueY;             // Additional value (Y)
} msg_struct;

//OLD struct
//typedef struct {
//	unsigned int id;       //airplane id
//	    char command[100];      //command
//	    int value;             //number to increment
//} msg_struct;




//Constructors & Destructors
OperatorConsole::OperatorConsole(planeManager & manager) : manager(&manager), runningOperatorConsole(false) {}

OperatorConsole::~OperatorConsole() {
	// TODO Auto-generated destructor stub
}

pthread_t OperatorConsole::getconsoleThread() const {
    return consoleThread;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void OperatorConsole::handleCommands() {
    std::string input;
    while (true) {
        // Display prompt for user input
        getline(cin, input);


//        if (input == "exit") {
//            std::cout << "Exiting Operator Console." << std::endl;
//            break;
//        }

        // Parse the input
        size_t firstSpace = input.find(' ');
        size_t secondSpace = input.find(' ', firstSpace + 1);
        size_t thirdSpace = input.find(' ', secondSpace + 1);

        // Check for correct input format
        if (firstSpace == std::string::npos || secondSpace == std::string::npos || thirdSpace == std::string::npos) {
            std::cerr << "Error: Invalid command format. Use: <AircraftID> <Command> <ValueX> <ValueY>\n";
            continue;
        }

        //Parsing input
        string idStr = input.substr(0, firstSpace);
        string commandStr = input.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        string valueXStr = input.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
        string valueYStr = input.substr(thirdSpace + 1);

        int aircraftID, valueX, valueY;


        try {
            aircraftID = std::stoi(idStr);
            valueX = std::stoi(valueXStr);
            valueY = std::stoi(valueYStr);
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid input values. Please enter valid integers!\n";
            continue;
        }

        //check airplane id
        int isValidID = manager->checkIds(aircraftID);
        if (!isValidID) {
            std::cerr << "Error: Aircraft ID " << aircraftID << " not found.\n";
            continue;
        }

        //Check if command valid
        Command command = stringToCommand(commandStr);
        if (command == Command::INVALID) {
            std::cerr << "Error: Invalid command: " << commandStr << "\n";
            continue;
        }


        std::cout << ">> Processing command '" << commandStr
                  << "' for Aircraft ID " << aircraftID
                  << " with ValueX: " << valueX
                  << " and ValueY: " << valueY << "\n";

        // Send the command
        sendCommand(aircraftID, commandStr, valueX, valueY);
    }
}





void OperatorConsole::sendCommand(int aircraftID, const std::string& command, int valueX, int valueY) {

	//1 Open channel
    int coid = name_open(CHANNEL_NAME, 0);
    if (coid == -1) {
        cerr << ERROR << "Operator failed to connect to channel: " << CHANNEL_NAME << endl << ERROR;
        return;
    }

    cout << ASTERISKS << "Operator connected to server on channel: " << CHANNEL_NAME << endl;

    //2 Prepare the message
    msg_struct msg;
    msg.id = aircraftID;
    strncpy(msg.command, command.c_str(), sizeof(msg.command) - 1);
    msg.command[sizeof(msg.command) - 1] = '\0';
    msg.valueX = valueX;
    msg.valueY = valueY;

    // Send the message
    msg_struct reply;
    if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
        perror("Error sending message");
        name_close(coid);
        return;
    }

    cout << "OperatorConsole: Received reply: " << reply.command << endl;

    name_close(coid);
}


//OLD

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
//	        size_t firstSpace = input.find(' ');
//	               size_t secondSpace = input.find(' ', firstSpace + 1);
//	               if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
//	                   cerr <<ERROR<< "Invalid command format. Use: <AircraftID> <Command> <Value>\n";
//	                   continue;
//	               }
//
//	       string idStr = input.substr(0, firstSpace);
//	                     string commandStr = input.substr(firstSpace + 1, secondSpace - firstSpace - 1);
//	                      string valueStr = input.substr(secondSpace + 1);
////
//
//	        	//check id
//	               int aircraftID,value;
//
//	               try {
//	            	   aircraftID = std::stoi(idStr);
//	            	   value = std::stoi(valueStr);
//	               } catch (const std::exception& e) {
//	                   cerr << ERROR << "Error: Invalid Airplane ID or value. Please enter a valid int number!\n"<< ERROR;
//	                   continue;
//	               }
//
//
//	               //check id is in the PlaneManager / is in shared memory
//	               int isValidID = manager->checkIds(aircraftID);
//	               if (!isValidID) {
//	                   cerr<< ERROR << ERROR<< "Error: Aircraft ID " << aircraftID << " not found in the airspace.\n"<< ERROR;
//	                   continue;
//	               }
//
//	               //is in enum list of commands
//	               Command command = stringToCommand(commandStr);
//	                      if (command == Command::INVALID) {
//	                          cerr << ERROR<< "Invalid command: " << commandStr << endl;
//	                          continue;
//	                      }
//
//	               //2
//	               //Send command
//	               cout << ">> Processing command '" << commandStr << "' for Aircraft ID " << aircraftID << " Value:  "<<value<<endl;
//
//	               sendCommand(aircraftID, commandStr, value);
//
//
//	    }
//}



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
//
//	    // Prepare the message
//	       msg_struct msg;
//	       msg.id = aircraftID;
//	       strncpy(msg.command, command.c_str(), sizeof(msg.command) - 1);
//	       msg.command[sizeof(msg.command) - 1] = '\0';
//	       msg.value = value;
//
//	       // Send the message
//	       msg_struct reply;
//	       if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
//	           perror("Error sending message");
//	           name_close(coid);
//	           return;
//	       }
//
//	       // Print the server's reply
//	       std::cout << "OperatorConsole: Received reply: " << reply.command <<endl;
//
//	       name_close(coid);
//
//}







