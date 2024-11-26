
#include "CommunicationSystem.h"
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <unistd.h>
#include <sys/dispatch.h>
#include <thread>

#include "airplane.h"
#include "planeManager.h"


#define SEPAR "\n\n------------------------------------------------------------------\n\n"

using namespace std;

//Message structure for IPC
typedef struct {
	unsigned int id;       //airplane id
	    char command[100];      //command
	    int value;             //number to increment
} msg_struct;





CommunicationSystem::CommunicationSystem(planeManager & manager) : manager(&manager) {}

CommunicationSystem::~CommunicationSystem(){}

///////////////////////////////////////////////////////////////////////////////////////////////

void CommunicationSystem::startCommunicationThread(){
	running = true;
	pthread_create(&communicationThread, nullptr, startServerThread, this);



}
pthread_t CommunicationSystem::getCommunicationThread() const{
		return communicationThread;
}

//
//server
void* CommunicationSystem::startServerThread(void* arg) {
    CommunicationSystem* system = static_cast<CommunicationSystem*>(arg);

    // Create a connection name for the server
    name_attach_t* attach = name_attach(NULL, "CommunicationSystemServer", 0);
    if (attach == NULL) {
        perror("name_attach");
        return nullptr;
    }

    std::cout << SEPAR << "CommunicationSystem: Server is running, waiting for messages..." << std::endl;

    while (true) {
        int rcvid;
        msg_struct msg; // Message received from the client

        // Receive a message
        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
        if (rcvid == -1) {
            perror("MsgReceive");
            continue;
        }

        if (rcvid == 0) {
            continue; // Ignore pulse messages
        }

        // Log the received message
        std::cout << SEPAR << "CommunicationSystem: Received command for Aircraft " << msg.id
                  << ": " << msg.command << " with value " << msg.value << SEPAR;

        // Process the command and call appropriate planeManager method
        std::string command(msg.command);
        if (command == "cs") {
            system->manager->changeSpeed(msg.id, msg.value);
        } else if (command == "ca") {
            system->manager->changeAltitude(msg.id, msg.value);
        } else if (command == "cp") {
            // Change position assumes direction change
            int x = msg.value;  // Assuming x and y values are encoded
            int y = msg.value;  // Modify based on actual structure
            system->manager->changeDirection(msg.id, x, y);
        } else if (command == "ct") {
            std::cout << "Command 'ct': Change time between collisions is not implemented yet." << std::endl;
        } else if (command == "pp") {
            system->manager->printPlane(msg.id);
        } else {
            std::cout << "Unknown command received: " << command << std::endl;
        }

        // Reply back to the client (acknowledge the message)
        msg_struct reply;
        reply.id = msg.id; // Echo back the ID
        strncpy(reply.command, "\nCommunicationSystem: Command Processed", sizeof(reply.command) - 1);
        reply.command[sizeof(reply.command) - 1] = '\0'; // Ensure null termination
        reply.value = 0; // Optional value for reply

        MsgReply(rcvid, 0, &reply, sizeof(reply));
    }

    // Detach the channel
    name_detach(attach, 0);
    return nullptr;
}



//OLD version
//void* CommunicationSystem::startServerThread(void* arg) {
//	// 1. Create a connection name for the server
//		    name_attach_t* attach = name_attach(NULL, "CommunicationSystemServer", 0);
//		    if (attach == NULL) {
//		        perror("name_attach");
//		        nullptr;
//		    }
//
//		    cout <<SEPAR << "CommunicationSystem: Server is running, waiting for messages...\n\n" ;
//
//		    while (true) {
//		    	        int rcvid;
//		    	        msg_struct msg; // Message received from the client
//
//		    	        // Receive a message
//		    	        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
//		    	        if (rcvid == -1) {
//		    	            perror("MsgReceive");
//		    	            continue;
//		    	        }
//
//		    	        if (rcvid == 0) {
//		    	            continue; // Ignore pulse messages
//		    	        }
//
//		    	        // Log the received message
//		    	        cout<<SEPAR<< "CommunicationSystem: Received command for Aircraft " << msg.id
//		    	                  << ": " << msg.command << " with value " << msg.value << endl <<SEPAR;
//
//		    //////////////////////////////////////////////////////////////////////////////////////
//		    	        //Block
//		    	        // Passing command to Plane
//		    	        //
//
//		    	        int tempId=msg.id;
//		    	        string tempCommand = msg.command;
//		    	        int value = msg.value;
//
//
//		    	       	//Block
//		    	        //////////////////////////////////////////////////////////////////////////////////////
//
//
//		    	        // Reply back to the client (acknowledge the message)
//		    	        msg_struct reply;
//		    	        reply.id = msg.id; // Echo back the ID
//		    	        strncpy(reply.command, "\nCommunicationSystem: Received Command", sizeof(reply.command) - 1);
//		    	        reply.command[sizeof(reply.command) - 1] = '\0'; // Ensure null termination
//		    	        reply.value = 0; // Optional value for reply
//
//		    	        MsgReply(rcvid, 0, &reply, sizeof(reply));
//
//		    //	        msg_struct reply = {msg.id};
//		    //	        strncpy(reply.command, "Acknowledged", sizeof(reply.command) - 1);
//		    //	        reply.command[sizeof(reply.command) - 1] = '\0';
//		    //	        reply.value = 0; // No processing, just acknowledgment
//		    //
//		    //	        MsgReply(rcvid, 0, &reply, sizeof(reply));
//		    	    }
//
//		    	    // Detach the channel
//		    	    name_detach(attach, 0);
//		    	    return nullptr;
//
//
//}









