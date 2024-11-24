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

typedef struct {
    unsigned int id;   // Identifier for the message (e.g., Aircraft ID)
    char body[100];    // Content of the message (e.g., command or data)
} msg_struct;



OperatorConsole::OperatorConsole() {


}

OperatorConsole::~OperatorConsole() {
	// TODO Auto-generated destructor stub
}

void OperatorConsole::sendCommand(int aircraftID, const std::string& command){

	//1.
	// Open the channel to connect to the Computer System (server)
	    int coid = name_open(CHANNEL_NAME, 0);
	    if (coid == -1) {
	        cerr << "Operator " << client_id << " failed to connect to channel: " << CHANNEL_NAME << endl;
	        return;
	    }

	    cout << "Operator " << client_id << " connected to server on channel: " << CHANNEL_NAME << endl;

	    // 2. Prepare the message structure
	       msg_struct msg;
	       msg.id = aircraftID;
	       strncpy(msg.body, command.c_str(), sizeof(msg.body) - 1);
	       msg.body[sizeof(msg.body) - 1] = '\0'; // Ensure null termination

	       std::cout << "OperatorConsole: Sending command to ComputerSystem: " << msg.body << std::endl;

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
	       std::cout << "OperatorConsole: Received reply from ComputerSystem: " << reply.body << std::endl;

	       // 6. Close the connection to the server
	       name_close(coid);
}

