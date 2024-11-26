#include "OperatorConsole.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>
#include <thread>
#include <string>
#include <sstream>

using namespace std;



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


mutex inputMutex;


///////////////////////////////////////////////////////////////////////////////////////////////////////////

//Constructors & Destructors
OperatorConsole::OperatorConsole(planeManager & manager) : manager(&manager), runningOperatorConsole(false) {}

OperatorConsole::~OperatorConsole() {
}

//Get thread
pthread_t OperatorConsole::getconsoleThread() const {
    return consoleThread;
}

//Start Thread
void OperatorConsole::startOperatorConsoleThread() {
    runningOperatorConsole = true;
    pthread_create(&consoleThread, nullptr, processCommands, this);
}

//Calls functions to process controller's command
void* OperatorConsole::processCommands(void* arg) {
    OperatorConsole* console = static_cast<OperatorConsole*>(arg);
    console->handleCommands();
    return nullptr;
}


//Reads commands from controller
void OperatorConsole::handleCommands() {
    string input;
    while (true) {

    	//Parsing input data from controller
    	//Getting id of plane, command and value from controller
    	//Based on the ammount of values written aftercommand (or lack thereod) we fill up the missing values with ones to stay consistent with msg_struct
    	//Here we are also checking if the commands are valid based on all the commands in Enum class
        getline(std::cin, input);

        istringstream iss(input);
        vector<string> tokens;
        string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.size() < 2 || tokens.size() > 4) {
            cerr << "ERROR: Invalid command format. Please refer to command menu\n";
            continue;
        }

        //Parsing input & putting ones if there is no values
        int aircraftID;
        string commandStr = tokens[1];
        int valueX = 1;
        int valueY = 1;

        try {
            aircraftID = std::stoi(tokens[0]);
            if (tokens.size() > 2) valueX = std::stoi(tokens[2]);
            if (tokens.size() > 3) valueY = std::stoi(tokens[3]);
        } catch (const std::exception& e) {
            cerr << "ERROR: Invalid input. Enter int values\n";
            continue;
        }

        //Check by airplane id in shared memory
        int isValidID = manager->checkIds(aircraftID);
        if (!isValidID) {
            std::cerr << "ERROR: Airplane with ID " << aircraftID << " is not in the airspace\n";
            continue;
        }

        //Validating command input
        Command command = stringToCommand(commandStr);
        if (command == Command::INVALID) {
            std::cerr << "ERROR: Invalid command " << commandStr << "\n";
            continue;
        }

        //Print to console
        cout << "\n>> Processing command '" << commandStr<< "' for Aircraft ID " << aircraftID<<endl<<endl;


        // Send the command
        sendCommand(aircraftID, commandStr, valueX, valueY);
    }
}



//Send Command to ComputerSystem
//Processes commands received & sends them in message to ComputerSystem
void OperatorConsole::sendCommand(int aircraftID, const std::string& command, int valueX, int valueY) {

	//1 Open channel
    int coid = name_open(CHANNEL_NAME, 0);
    if (coid == -1) {
        cerr << ERROR << "Operator failed to connect to channel: " << CHANNEL_NAME << endl << ERROR;
        return;
    }

    //2 Prepare the message
    msg_struct msg;
    msg.id = aircraftID;
    strncpy(msg.command, command.c_str(), sizeof(msg.command) - 1);
    msg.command[sizeof(msg.command) - 1] = '\0';
    msg.valueX = valueX;
    msg.valueY = valueY;

    //3 Send the message
    msg_struct reply;
    if (MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1) {
        perror("Error sending message");
        name_close(coid);
        return;
    }


    //4 Close channel
    name_close(coid);
}




