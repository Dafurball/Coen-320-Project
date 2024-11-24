#include "OperatorConsole.h"
#include <iostream>
#include <cstring>
#include <sstream>

#define CHANNEL_NAME "ComputerSystemServer"

typedef struct {
    unsigned int id;   // Identifier for the message (e.g., Aircraft ID)
    char body[100];    // Content of the message (e.g., command or data)
} msg_struct;

OperatorConsole::OperatorConsole() : running(false) {}

OperatorConsole::~OperatorConsole() {
    stopOperatorConsoleThread();
}

void OperatorConsole::startOperatorConsoleThread() {
    running = true;
    input_thread = std::thread(&OperatorConsole::inputLoop, this);
}

void OperatorConsole::stopOperatorConsoleThread() {
    running = false;
    if (input_thread.joinable()) {
        input_thread.join();
    }
}

void OperatorConsole::inputLoop() {
    while (running) {
        std::string input;
        std::cout << "Enter command (format: <AircraftID> <Command>): ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        std::istringstream iss(input);
        int aircraftID;
        std::string command;

        if (!(iss >> aircraftID >> command)) {
            std::cerr << "Invalid input format. Use: <AircraftID> <Command>" << std::endl;
            continue;
        }

        sendCommand(aircraftID, command);
    }
}

void OperatorConsole::sendCommand(int aircraftID, const std::string& command) {
    int coid = name_open(CHANNEL_NAME, 0);
    if (coid == -1) {
        std::cerr << "Operator failed to connect to channel: " << CHANNEL_NAME << std::endl;
        return;
    }

    msg_struct msg;
    msg.id = aircraftID;
    strncpy(msg.body, command.c_str(), sizeof(msg.body) - 1);
    msg.body[sizeof(msg.body) - 1] = '\0';

    msg_struct reply;

    int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
    if (status == -1) {
        perror("MsgSend failed");
        name_close(coid);
        return;
    }

    std::cout << "OperatorConsole: Received reply from ComputerSystem: " << reply.body << std::endl;
    name_close(coid);
}
