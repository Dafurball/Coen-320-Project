
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
	// 1. Create a connection name for the server
		    name_attach_t* attach = name_attach(NULL, "CommunicationSystemServer", 0);
		    if (attach == NULL) {
		        perror("name_attach");
		        nullptr;
		    }

		    cout << "CommunicationSystem: Server is running, waiting for messages...\n\n" ;

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
		    	        cout<< ">> CommunicationSystem: Received command for Aircraft " << msg.id
		    	                  << ": " << msg.command << " with value " << msg.value << endl;

		    //////////////////////////////////////////////////////////////////////////////////////
		    	        //Block
		    	        // Passing command to Plane
		    	        //

		    	        int tempId=msg.id;
		    	        string tempCommand = msg.command;
		    	        int value = msg.value;


		    	       	//Block
		    	        //////////////////////////////////////////////////////////////////////////////////////


		    	        // Reply back to the client (acknowledge the message)
		    	        msg_struct reply;
		    	        reply.id = msg.id; // Echo back the ID
		    	        strncpy(reply.command, ">> CommunicationSystem: Received Command", sizeof(reply.command) - 1);
		    	        reply.command[sizeof(reply.command) - 1] = '\0'; // Ensure null termination
		    	        reply.value = 0; // Optional value for reply

		    	        MsgReply(rcvid, 0, &reply, sizeof(reply));

		    //	        msg_struct reply = {msg.id};
		    //	        strncpy(reply.command, "Acknowledged", sizeof(reply.command) - 1);
		    //	        reply.command[sizeof(reply.command) - 1] = '\0';
		    //	        reply.value = 0; // No processing, just acknowledgment
		    //
		    //	        MsgReply(rcvid, 0, &reply, sizeof(reply));
		    	    }

		    	    // Detach the channel
		    	    name_detach(attach, 0);
		    	    return nullptr;


}










